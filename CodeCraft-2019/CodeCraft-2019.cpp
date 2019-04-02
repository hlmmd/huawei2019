#include "iostream"
#include <vector>
#include <sys/time.h>
#include "common.h"

using std::vector;
using std::cout;
using std::endl;
using std::min;

int global_flag = 0;

int main(int argc, char *argv[])
{
	srand(time(0));
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	gettimeofday(&start, NULL);

	if (argc < 5)
	{
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);

	//读入数据，初始化
	ReadCar(Car::Cars, carPath);
	for (int i = 0; i < Car::Cars.size(); i++)
	{
		CarNameSpace.push_back(Car::Cars[i].id);
		CarDict[Car::Cars[i].id] = &Car::Cars[i];
		Optimize::speed_set.insert(Car::Cars[i].maxspeed);
	}
	int work = 0;
	for (auto s : Optimize::speed_set)
	{
		Optimize::speed_map[s] = work;
		work++;
	}

	ReadRoad(Road::Roads, roadPath);
	for (int i = 0; i < Road::Roads.size(); i++)
	{
		RoadNameSpace.push_back(Road::Roads[i].id);
		RoadDict[Road::Roads[i].id] = &Road::Roads[i];
	}

	ReadCross(Cross::Crosses, crossPath);
	for (int i = 0; i < Cross::Crosses.size(); i++)
	{
		CrossNameSpace.push_back(Cross::Crosses[i].id);
		CrossDict[Cross::Crosses[i].id] = &Cross::Crosses[i];
	}

	sort(CarNameSpace.begin(), CarNameSpace.end());
	sort(CrossNameSpace.begin(), CrossNameSpace.end());

	//在去除单项路之前计算每个路口的车道总数
	int channel_total = 0;
	for (auto &cross : Cross::Crosses)
		channel_total += cross.Cal_road_channel_num();

	for (auto &cross : Cross::Crosses)
		cross.RemoveSingleRoad();

	int time = 1;

	auto comp = [](Car car1, Car car2) {
		//	if (car1.start_time == car2.start_time)
		return car1.maxspeed > car2.maxspeed;
		//		return car1.start_time < car2.start_time;
	};
	std::sort(Car::Cars.begin(), Car::Cars.end(), comp);

	//先计算一次最短路径
	for (auto &car : Car::Cars)
	{
		car.dj_time = car.CalDijkstraPath();
		time += car.dj_time;
		car.set_dir_type();
	}

	//在计算过一次最短路径后，重新计算时讲之前的数据清零。
	for (auto &cross : Cross::Crosses)
	{
		cross.cars_pass_num = 0;
	}
	for (auto &car : Car::Cars)
	{
		car.dir_seq.clear();
		car.road_seq.clear();
	}
	time = 0;
	for (auto &car : Car::Cars)
	{
		if (car.is_dir_type_set)
		{
			time += car.CalDijkstraPath_withdir();
		}
		else
		{
			int ret = -1;
			//如果没找到路，则更换方向，如果换了3次都没找到，说明出错。
			for (int i = 0; i < 4; i++)
			{
				ret = car.CalDijkstraPath_withdir();
				if (ret > 0)
					break;
				car.get_next_dir_type();
			}
			//存在找不到的情况，因为不一定有路，且道路不一定双向。
			if (ret == -1)
				time += car.CalDijkstraPath();
			else
			{
				car.is_dir_type_set = true;
				time += ret;
			}
		}
	}

	//根据行驶方向分成四个组
	const int Dir_group_size = 4;
	std::vector<std::vector<Car>> Cars_group(Dir_group_size);
	Divide_Group(Cars_group);

	//统计所有车的车速，并降序排列
	double car_speed_avg = 0;
	std::vector<int> car_speed;
	for (auto car : Car::Cars)
	{
		car_speed_avg += car.maxspeed;
		if (std::find(car_speed.begin(), car_speed.end(), car.maxspeed) == car_speed.end())
			car_speed.push_back(car.maxspeed);
	}
	car_speed_avg /= Car::Cars.size();
	auto comp_speed = [](int s1, int s2) { return s1 > s2; };
	std::sort(car_speed.begin(), car_speed.end(), comp_speed);

	//根据车速再分组，车速快的最先发车，再发慢的。最大化利用车速优势。
	std::vector<std::vector<std::vector<Car>>> Cars_dir_speed_group;

	for (int n = 0; n < Dir_group_size; n++)
	{
		std::vector<int> map(Dir_group_size, 0);
		map[0] = 0, map[1] = 2, map[2] = 1, map[3] = 3;
		std::vector<std::vector<Car>> Cars_speed_group;
		Divide_speed_Group(Cars_group[map[n]], Cars_speed_group, car_speed);
		Cars_dir_speed_group.push_back(Cars_speed_group);
	}

	int min_start_time = Car::Cars[0].start_time;
	for (auto car : Car::Cars)
	{
		if (min_start_time > car.start_time)
			min_start_time = car.start_time;
	}

	int schdule_time = min_start_time;

	int delta_time = 1;
	//csbeg  update value1
	int channel_avg = 0;
	for (auto road : Road::Roads)
		channel_avg += road.channel;
	channel_avg /= Road::Roads.size();

	double div2 = Car::Cars.size() / 10000 / 2;
	if (div2 < 1)
		div2 = 1;
	std::vector<int> cars_ttl;

	int new_cross_channels = 0;
	for (int i = 0; i < Cross::Crosses.size(); i++)
	{
		for (int j = 0; j < ROADS_OF_CROSS; j++)
		{
			if (Cross::Crosses[i].dir[j] != -1)
				new_cross_channels += Road::Roads[Road_findpos_by_id(Cross::Crosses[i].dir[j])].channel;
		}
	}
	new_cross_channels /= 2;
	int new_total_vol = 0;
	for (auto road : Road::Roads)
	{
		new_total_vol += road.length * road.channel * (road.is_dup + 1);
	}
	new_total_vol /= 2;

	std::vector<double> new_total_djtime_group(car_speed.size(), 0);
	for (auto car : Car::Cars)
	{
		for (int i = 0; i < car_speed.size(); i++)
		{
			if (car.maxspeed == car_speed[i])
			{
				new_total_djtime_group[i] += car.dj_time;
				break;
			}
		}
	}

	double car_djtime_avg = 0;
	for (int j = 0; j < car_speed.size(); j++)
	{
		for (int i = 0; i < Dir_group_size; i++)
		{

			std::unordered_map<int, int> start_cross_id;
			int start_cross_channels = 0;
			for (auto car : Cars_dir_speed_group[i][j])
			{
				if (start_cross_id.find(car.src) == start_cross_id.end())
					start_cross_id[car.src] = 1;
				else
					start_cross_id[car.src]++;
			}
			std::vector<int> start_cross_id_vector;
			for (auto it = start_cross_id.begin(); it != start_cross_id.end(); it++)
			{
				if (it->second >= (Cars_dir_speed_group[i][j].size() / 70))
					start_cross_id_vector.push_back(it->first);
				//cout << it->first << " " << it->second << endl;
			}

			for (auto cross_id : start_cross_id_vector)
			{
				for (int i = 0; i < ROADS_OF_CROSS; i++)
					if (Cross::Crosses[Cross_findpos_by_id(cross_id)].dir[i] != -1)
						start_cross_channels++;
			}

			int total_vol = 0;
			for (auto cross_id : start_cross_id_vector)
			{
				for (int i = 0; i < ROADS_OF_CROSS; i++)
					if (Cross::Crosses[Cross_findpos_by_id(cross_id)].dir[i] != -1)
					{
						int pos = Road_findpos_by_id(Cross::Crosses[Cross_findpos_by_id(cross_id)].dir[i]);
						total_vol += Road::Roads[pos].channel * Road::Roads[pos].length;
					}
			}

			int start_chnnel = 0;
			int started_car_nums = 0;
			//计算这些车的平均用时，作为relax_time，表示跑完发完一轮车
			int t = 0;
			for (auto car : Cars_dir_speed_group[i][j])
			{
				if (car.started == false)
					t += car.dj_time;
			}
			double relax_time = t / Cars_dir_speed_group[i][j].size();

			int car_roadnum_avg = 0;
			int start_per_time;
			while (!finish_start_group(Cars_dir_speed_group[i][j]))
			{
				//当前车辆组中未发车的数量
				int cars_size = Cars_dir_speed_group[i][j].size() - started_car_nums;

				car_djtime_avg = 0;
				car_roadnum_avg = 0;
				for (auto car : Cars_dir_speed_group[i][j])
				{
					if (car.started == false)
					{
						car_djtime_avg += car.dj_time;
						car_roadnum_avg += car.road_seq.size();
					}
				}
				car_roadnum_avg /= cars_size;
				double div = car_speed_avg; //car_speed_avg
				car_djtime_avg /= cars_size;
				start_per_time = start_cross_id_vector.size() * sqrt(sqrt(car_speed[j]));
				if (j <= 2 && start_per_time > 35)
				{
					start_per_time = 35;
				}
				else if (start_per_time > 30)
					start_per_time = 30;
				else if (start_per_time < 20)
					start_per_time = 20;
				//	int start_per_time = (double)relax_time * relax_time / car_djtime_avg * cars_size * Cars_dir_speed_group[i][j][0].maxspeed / div * 2.5;

				int start_index = 0;
				for (; start_index < Cars_dir_speed_group[i][j].size(); start_index++)
					if (Cars_dir_speed_group[i][j][start_index].started == false)
						break;

				int temp_start_count = 0;
				for (int x = 0; x < start_per_time && started_car_nums < Cars_dir_speed_group[i][j].size() && start_index < Cars_dir_speed_group[i][j].size(); start_index++)
				{
					//只有没有开的车、且计划出发时间小于当前安排时间的会被访问
					if (Cars_dir_speed_group[i][j][start_index].started == false && Cars_dir_speed_group[i][j][start_index].start_time <= (schdule_time))
					{
						Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].start_time = (schdule_time);
						Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].started = true;
						Cars_dir_speed_group[i][j][start_index].started = true;
						started_car_nums++;
						x++;
						temp_start_count++;
					}
				}

				//考虑任然存在start_time>schdule_time的车 感觉写的全是bug
				start_index = 0;
				while (temp_start_count < start_per_time && started_car_nums < Cars_dir_speed_group[i][j].size())
				{
					if (Cars_dir_speed_group[i][j][start_index].started == false)
					{
						//Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].start_time = (schdule_time);
						Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].started = true;
						Cars_dir_speed_group[i][j][start_index].started = true;
						started_car_nums++;
						temp_start_count++;
					}
					start_index++;
				}
				schdule_time += delta_time;
				cout << start_per_time << " " << schdule_time << endl;
			}

			schdule_time += car_djtime_avg * 2;
		}
	}

	//	std::cout << time << std::endl;

	auto comp2 = [](Car car1, Car car2) { return car1.id < car2.id; };
	std::sort(Car::Cars.begin(), Car::Cars.end(), comp2);
	WriteAnswer(Car::Cars, answerPath);
	exit(0);
	//zhao
	std::ifstream fin(answerPath, std::ios::in);
	if (fin)
	{
		std::string line;
		while (getline(fin, line))
		{
			vector<std::string> res;
			std::string result;

			std::stringstream ss(line);
			while (getline(ss, result, ','))
			{
				result = num_trim(result);
				res.push_back(result);
			}

			int carId = stoi(res[0]);
			int planTime = stoi(res[1]);
			vector<int> route;
			for (int i = 2; i < res.size(); i++)
			{
				route.push_back(stoi(res[i]));
			}
			CarDict[carId]->startInit(planTime, route);
		}
	}
	else
	{
		std::cout << "open error" << std::endl;
		return -1;
	}
	fin.close();
	CarDistribution[0] = CarNameSpace.size();
	for (int carId : CarNameSpace)
	{
		CrossDict[CarDict[carId]->src]->carportInital(CarDict[carId]->plane_time, carId);
	}
	sort(CarNameSpace.begin(), CarNameSpace.end());
	sort(CrossNameSpace.begin(), CrossNameSpace.end());

	//运行判题器
	//	while (1)
	// {
	Simulation s;
	int simulate_time = s.simulate();
	//Car::Cars = Car::Answer_bk; //
	s.init();
	std::cout << "schdule time: " << simulate_time << std::endl;
	// }

	exit(0);

	global_flag = 1;

	gettimeofday(&end, NULL);
	diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	std::cout << "program time: " << diff << std::endl;

	exit(0);

	Optimize optimizer;
	Optimize::dist_time.assign(Optimize::speed_map.size(), std::vector<std::vector<double>>(Cross::Crosses.size(), std::vector<double>(Cross::Crosses.size(), 9999999)));
	Optimize::scores.assign(Road::Roads.size(), std::vector<double>(Cross::Crosses.size(), 0));
	Optimize::dist_table.assign(Optimize::speed_map.size(), std::vector<std::vector<std::vector<int>>>(Cross::Crosses.size(), std::vector<std::vector<int>>(Cross::Crosses.size(), std::vector<int>())));

	//这里求出两个表，表太大不用函数调用
	//n*n vector<int>存储对应两点的最短陆径，弗洛伊德算法
	for (auto s : Optimize::speed_map)
	{
		for (auto road : Road::Roads)
		{
			Optimize::dist_time[s.second][Cross_findpos_by_id(road.src_cross)][Cross_findpos_by_id(road.dst_cross)] = (double(road.length) / min(s.first, road.maxspeed));
			Optimize::dist_table[s.second][Cross_findpos_by_id(road.src_cross)][Cross_findpos_by_id(road.dst_cross)].push_back(road.id);
			if (road.is_dup)
			{
				Optimize::dist_time[s.second][Cross_findpos_by_id(road.dst_cross)][Cross_findpos_by_id(road.src_cross)] = (double(road.length) / min(s.first, road.maxspeed));
				Optimize::dist_table[s.second][Cross_findpos_by_id(road.dst_cross)][Cross_findpos_by_id(road.src_cross)].push_back(road.id);
			}
		}
	}

	for (auto s : Optimize::speed_map)
	{ //O(cross^3*speeds)
		for (int i1 = 0; i1 < Cross::Crosses.size(); ++i1)
		{
			for (int j1 = 0; j1 < Cross::Crosses.size(); ++j1)
			{
				for (int k1 = 0; k1 < Cross::Crosses.size(); ++k1)
				{
					if (Optimize::dist_time[s.second][j1][k1] > Optimize::dist_time[s.second][j1][i1] + Optimize::dist_time[s.second][i1][k1])
					{
						Optimize::dist_time[s.second][j1][k1] = Optimize::dist_time[s.second][j1][i1] + Optimize::dist_time[s.second][i1][k1];
						Optimize::dist_table[s.second][j1][k1].assign(Optimize::dist_table[s.second][j1][i1].begin(), Optimize::dist_table[s.second][j1][i1].end());
						for (auto t : Optimize::dist_table[s.second][i1][k1])
						{
							Optimize::dist_table[s.second][j1][k1].push_back(t);
						}
					}
				}
			}
		}
	}

	int min_time = 999999;

	//	Car::Answer_bk = Car::Cars;
	Car::Answer_bk.assign(Car::Cars.begin(), Car::Cars.end());

	Simulation sim;
	int test = 50;
	while (test--)
	{
		if (test == 20)
		{
			int l = 1;
		}
		int time_sche = sim.simulate();
		cout << "finish time:" << time_sche << "  Test:" << test << endl;
		if (time_sche == -1)
		{
			cout << "dead by cs";

			Car::Cars.assign(Car::Answer_bk.begin(), Car::Answer_bk.end());
			//Car::Cars = Car::Answer_bk; //
			sim.init();
			continue;
		}
		optimizer.update_scores(time_sche);
		if (time_sche < min_time)
		{
			//
			Car::Answer.assign(Car::Cars.begin(), Car::Cars.end());
			Car::Answer[0].Display();
			//Car::Answer = Car::Cars;
			min_time = time_sche;
		}

		Car::Cars.assign(Car::Answer_bk.begin(), Car::Answer_bk.end());
		//Car::Cars = Car::Answer_bk;
		sim.init();
	}
	cout << "min_time:" << min_time << endl;
	Car::Answer[0].Display();
	WriteAnswer2(Car::Answer, answerPath);

	return 0;
}
