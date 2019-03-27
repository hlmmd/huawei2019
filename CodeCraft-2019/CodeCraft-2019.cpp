#include "iostream"
#include <vector>
#include <sys/time.h>
#include "common.h"

using std::vector;

using std::cout;
using std::endl;

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
		Cross::Crosses[i].InitSimulate();
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
		if (car1.start_time == car2.start_time)
			return car1.maxspeed > car2.maxspeed;
		return car1.start_time < car2.start_time;
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

	//当前地图最大出发时间为10，直接将初始调度时间设置为10，则不需要考虑出发时间的影响。

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
		channel_avg += road.channel << road.is_dup;
	channel_avg /= Road::Roads.size();

	//	int div = sqrt(channel_total) * delta_time/2;

	for (int j = 0; j < car_speed.size(); j++)
	{
		for (int i = 0; i < Dir_group_size; i++)
		{
			//std::cout << Cars_dir_speed_group[i][j].size() <<" " <<Cars_dir_speed_group[i][j][0].maxspeed << std::endl;
			//continue;
			int started_car_nums = 0;
			//计算这些车的平均用时，作为relax_time，表示跑完发完一轮车
			int t = 0;
			for (auto car : Cars_dir_speed_group[i][j])
			{
				if (car.started == false)
					t += car.dj_time;
			}
			double relax_time = t / Cars_dir_speed_group[i][j].size();
			double car_djtime_avg = 0;
			int car_roadnum_avg = 0;

			int min_dj_time = INT_MAX;
			for (auto car : Cars_dir_speed_group[i][j])
			{
				if (car.dj_time < min_dj_time)
					min_dj_time = car.dj_time;
			}

			while (!finish_start_group(Cars_dir_speed_group[i][j]))
			{
				//当前车辆组中未发车的数量
				int cars_size = Cars_dir_speed_group[i][j].size() - started_car_nums;

				car_djtime_avg = 0;
				car_roadnum_avg = 0;
				for (auto car : Cars_dir_speed_group[i][j])
				{
					if (car.started == false  )
					{
						car_djtime_avg += car.dj_time;
						car_roadnum_avg += car.road_seq.size();
					}
				}

				car_roadnum_avg /= cars_size;
				double div = car_speed_avg; //car_speed_avg
				//int start_per_time = (double)car_djtime_avg / cars_size * Cars_dir_speed_group[i][j][0].maxspeed / div;
				int start_per_time = (double)relax_time*relax_time/car_djtime_avg * cars_size * Cars_dir_speed_group[i][j][0].maxspeed / div * 2.5;
				car_djtime_avg /= cars_size;

				int start_index = 0;
				for (; start_index < Cars_dir_speed_group[i][j].size(); start_index++)
					if (Cars_dir_speed_group[i][j][start_index].started == false)
						break;

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
					}
				}
				//考虑任然存在start_time>schdule_time的车
				for (int x = 0; x < Cars_dir_speed_group[i][j].size(); x++)
				{
					if (Cars_dir_speed_group[i][j][start_index].started == false)
					{
						//Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].start_time = (schdule_time);
						Car::Cars[Car_findpos_by_id(Cars_dir_speed_group[i][j][start_index].id)].started = true;
						Cars_dir_speed_group[i][j][start_index].started == true;
					}
				}

			//	std::cout << start_per_time << " " << schdule_time << std::endl;
				schdule_time += delta_time;
			}

			//schdule_time += car_djtime_avg / 6;
			if (j == 0)
				schdule_time += car_djtime_avg / 4;
			else if (j == 1)
				schdule_time += car_djtime_avg / 6;
			else if (j == 2)
				schdule_time += car_djtime_avg / 3.3;
			else if (j == 3)
				schdule_time += car_djtime_avg / 3.7;
			//schdule_time += car_djtime_avg / 16;
		}
	}
	//	std::cout << time << std::endl;

	auto comp2 = [](Car car1, Car car2) { return car1.id < car2.id; };
	std::sort(Car::Cars.begin(), Car::Cars.end(), comp2);
	WriteAnswer(Car::Cars, answerPath);

	//运行判题器
	Simulation s;
	s.init();

	int simulate_time = s.simulate();
	std::cout << "schdule time: " << simulate_time << std::endl;

	gettimeofday(&end, NULL);
	diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	std::cout << "program time: " << diff << std::endl;

	exit(0);

	Optimize optimizer;

	Optimize::dist.assign(Cross::Crosses.size(), std::vector<int>(Cross::Crosses.size(), 9999999));
	Optimize::scores.assign(Road::Roads.size(), std::vector<int>(Cross::Crosses.size(), 0));
	Optimize::dist_table.assign(Cross::Crosses.size(), std::vector<std::vector<int>>(Cross::Crosses.size(), std::vector<int>()));
	//这里求出两个表，表太大不用函数调用
	//n*n vector<int>存储对应两点的最短陆径，弗洛伊德算法
	for (auto road : Road::Roads)
	{
		Optimize::dist[Cross_findpos_by_id(road.src_cross)][Cross_findpos_by_id(road.dst_cross)] = road.length;
		Optimize::dist_table[Cross_findpos_by_id(road.src_cross)][Cross_findpos_by_id(road.dst_cross)].push_back(road.id);
		if (road.is_dup)
		{
			Optimize::dist[Cross_findpos_by_id(road.dst_cross)][Cross_findpos_by_id(road.src_cross)] = road.length;
			Optimize::dist_table[Cross_findpos_by_id(road.dst_cross)][Cross_findpos_by_id(road.src_cross)].push_back(road.id);
		}
	}

	for (int i1 = 0; i1 < Cross::Crosses.size(); ++i1)
	{
		for (int j1 = 0; j1 < Cross::Crosses.size(); ++j1)
		{
			for (int k1 = 0; k1 < Cross::Crosses.size(); ++k1)
			{
				if (Optimize::dist[j1][k1] > Optimize::dist[j1][i1] + Optimize::dist[i1][k1])
				{
					Optimize::dist[j1][k1] = Optimize::dist[j1][i1] + Optimize::dist[i1][k1];
					Optimize::dist_table[j1][k1].assign(Optimize::dist_table[j1][i1].begin(), Optimize::dist_table[j1][i1].end());
					for (auto t : Optimize::dist_table[i1][k1])
					{
						Optimize::dist_table[j1][k1].push_back(t);
					}
				}
			}
		}
	}

	Simulation sim;
	sim.init();
	int time_sche = sim.simulate();
	if (time_sche == -1)
	{
		cout << "dead";
		exit(-1);
	}
	cout << time_sche << endl;
	//csbeg  update value1

	//利用之前的Cars，是一个可行解，更新Road.score
	int road_pos, last_score, dst_pos;
	for (auto car : Car::Cars)
	{
		for (auto i : car.road_seq)
		{
			road_pos = Road_findpos_by_id(i);
			dst_pos = Cross_findpos_by_id(car.dst);
			last_score = Optimize::scores[road_pos][dst_pos];
			Optimize::scores[road_pos][dst_pos] = (last_score == 0) ? time_sche : (last_score + time_sche) / 2; /////?
		}
	}
	//Reset_Group(Cars_group);

	//计算3（2）？个方向的下一个cross的分数
	//1、距离目标dst的dist 直接读取dist[src][dst]
	//2、当前路径上车辆cars cars_speed  该状态动态更新
	//3、当前路径状态 int sit_score=Road::Roads[Road_findpos_by_id(id)].score;
	//在去除单项路之前计算每个路口的车道总数
	//std::vector<std::vector<std::vector<std::string>>>dist_table(Cross::Crosses.size(),std::vector<std::vector<std::string>>(Cross::Crosses.size(),std::vector<std::string>()));

	//csend

	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file

	return 0;
}
