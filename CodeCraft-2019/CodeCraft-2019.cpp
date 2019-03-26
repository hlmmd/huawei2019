#include "iostream"
#include <vector>
#include <sys/time.h>
#include "common.h"

int main(int argc, char *argv[])
{
	srand(time(0));
	struct timeval start;
	struct timeval end;
	unsigned long diff;

	gettimeofday(&start, NULL);

	//	std::cout << "Begin" << std::endl;

	if (argc < 5)
	{
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);

	// std::cout << "carPath is " << carPath << std::endl;
	// std::cout << "roadPath is " << roadPath << std::endl;
	// std::cout << "crossPath is " << crossPath << std::endl;
	// std::cout << "answerPath is " << answerPath << std::endl;

	ReadCar(Car::Cars, carPath);

	ReadRoad(Road::Roads, roadPath);

	ReadCross(Cross::Crosses, crossPath);

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
		//	car.Display();
		//car.start_time = time;
		car.dj_time = car.CalDijkstraPath();
		time += car.dj_time;
		car.set_dir_type();
		//	std::cout<<time<<" ";
		//		car.Display();
		//std::cout<<car.road_seq.size()<<" ";
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
	int count = 0;
	for (auto &car : Car::Cars)
	{
		car.start_time = time;

	//		std::cout << car.dir_type << " " << car.is_dir_type_set << std::endl;
		if (car.is_dir_type_set)
		{
			count++;
			time += car.CalDijkstraPath_withdir();
		}
		else
		{
			int ret = -1;
			//如果没找到路，则更换方向，如果换了3次都没找到，说明出错。
			for (int i = 0; i < 4; i++)
			{
			//	std::cout << "cal" << std::endl;
				ret = car.CalDijkstraPath_withdir();
				if (ret > 0)
					break;
				car.get_next_dir_type();
			}
			//存在找不到的情况，因为不一定有路，且道路不一定双向。
			if (ret == -1)
			{
				time+=car.CalDijkstraPath();

				// std::cout << "not found" << std::endl;
				// exit(0);
			}
			else
			{
				count++;
				car.is_dir_type_set = true;
		//		std::cout << ret << std::endl;
				time += ret;
			}

			for (int i = 0; i < car.dir_seq.size(); i++)
			{
				std::cout << car.dir_seq[i] << " ";
			}
		//	std::cout << car.dir_type << std::endl;
		//	exit(0);
		}
		//	exit(0);
	}


	std::vector<std::vector<Car>> Cars_group(4);

	Divide_Group(Cars_group);

	// 	for(int i = 0;i<Cars_group.size();i++)
	// 	{
	// 		std::vector<std::vector<Car>> Cars_speed_group;
	// Divide_speed_Group(Cars_group[i],Cars_speed_group);
	// 	}
	// 	exit(0);

	//当前地图最大出发时间为10，直接将初始调度时间设置为10，则不需要考虑出发时间的影响。

	int schdule_time = 10;
	int delta_time = 1;
	//	int relax_time = time / Car::Cars.size();

	//int div = sqrt(Cross::Crosses.size()) / 3;
	//	int div = sqrt(channel_total)/delta_time;

	int channel_avg = 0;
	for (auto road : Road::Roads)
		channel_avg += road.channel << road.is_dup;
	channel_avg /= Road::Roads.size();

	//	int div = sqrt(channel_total) * delta_time/2;
	for (int n = 0; n < 4; n++)
	{
		std::vector<std::vector<Car>> Cars_speed_group;
		Divide_speed_Group(Cars_group[n], Cars_speed_group);

		for (int i = 0; i < 4; i++)
		{
			int started_car_nums = 0;

			//计算这些车的平均用时，作为relax_time，表示跑完发完一轮车
			int t = 0;
			for (auto car : Cars_speed_group[i])
			{
				if (car.started == false)
					t += car.dj_time;
			}
			int relax_time = t / Cars_speed_group[i].size();
			int car_djtime_avg = 0;
			int car_roadnum_avg = 0;
			while (!finish_start_group(Cars_speed_group[i]))
			{
				//当前车辆组中未发车的数量
				int cars_size = Cars_speed_group[i].size() - started_car_nums;

				int cc = 0;
				for (auto car : Cars_speed_group[i])
				{
					if (car.started == false)
					{
						cc++;
						car_djtime_avg += car.dj_time;
						car_roadnum_avg += car.road_seq.size();
					}
				}
				//std::cout << cars_size <<" " <<cc << std::endl;
				car_djtime_avg /= cars_size;
				car_roadnum_avg /= cars_size;
				//int start_per_time = (double)Cars_speed_group[i].size() / car_roadnum_avg * delta_time;
				//int start_per_time = (double)Cars_speed_group[i].size() / car_djtime_avg * delta_time;

				int start_per_time = car_djtime_avg * delta_time;
				if (start_per_time < relax_time)
					start_per_time = relax_time;
				//std::cout << cars_size << " " << start_per_time << std::endl;
				// if(start_per_time<cars_size)
				// 	start_per_time = cars_size;
				//std::cout << cars_size <<" " <<cc <<" " << start_per_time <<" " <<car_djtime_avg<<" "<<car_roadnum_avg<<std::endl;
				for (int x = 0; x < start_per_time && started_car_nums < Cars_speed_group[i].size(); x++, started_car_nums++)
				{
					//std::cout<<"A";
					Cars_speed_group[i][started_car_nums].started = true;
					Cars_speed_group[i][started_car_nums].start_time = schdule_time;

					Car::Cars[Car_findpos_by_id(Cars_speed_group[i][started_car_nums].id)].start_time = (schdule_time);

					// if (Car::Cars[Car_findpos_by_id(Cars_group[i][started_car_nums].id)].start_time + 9 < schdule_time)
					// 	Car::Cars[Car_findpos_by_id(Cars_group[i][started_car_nums].id)].start_time = schdule_time - 9;
					// else
					// 	Car::Cars[Car_findpos_by_id(Cars_group[i][started_car_nums].id)].start_time = (schdule_time - 9);
				}
				//for(int i=0;i<Cars_group[n].size();)
		//		std::cout << start_per_time << " " << schdule_time << std::endl;
				schdule_time += delta_time;
			}

			if (i == 0)
				schdule_time += car_djtime_avg / 2 ;
			else if (i == 1)
				schdule_time += car_djtime_avg / 2 ;
			else if (i == 2)
				schdule_time += car_djtime_avg / 2;
			else if (i == 3)
				schdule_time += car_djtime_avg /2;

			// if (i == 0)
			// 	schdule_time += car_djtime_avg / 8;
			// else if (i == 1)
			// 	schdule_time += car_djtime_avg / 4;
			// else if (i == 2)
			// 	schdule_time += car_djtime_avg / 2;
			// else if (i == 3)
			// 	schdule_time += car_djtime_avg * 1.5;
		}
		//schdule_time+= time/Car::Cars.size()*5;
	}

	/*发现没有必要重新计算。

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


	int count = 0;
	for (auto &car : Car::Cars)
	{
		car.start_time = time;

	//		std::cout << car.dir_type << " " << car.is_dir_type_set << std::endl;
		if (car.is_dir_type_set)
		{
			count++;
			time += car.CalDijkstraPath_withdir();
		}
		else
		{
			int ret = -1;
			//如果没找到路，则更换方向，如果换了3次都没找到，说明出错。
			for (int i = 0; i < 4; i++)
			{
			//	std::cout << "cal" << std::endl;
				ret = car.CalDijkstraPath_withdir();
				if (ret > 0)
					break;
				car.get_next_dir_type();
			}
			//存在找不到的情况，因为不一定有路，且道路不一定双向。
			if (ret == -1)
			{
				time+=car.CalDijkstraPath();

				// std::cout << "not found" << std::endl;
				// exit(0);
			}
			else
			{
				count++;
				car.is_dir_type_set = true;
				std::cout << ret << std::endl;
				time += ret;
			}

			for (int i = 0; i < car.dir_seq.size(); i++)
			{
				std::cout << car.dir_seq[i] << " ";
			}
			std::cout << car.dir_type << std::endl;
		//	exit(0);
		}
		//	exit(0);
	}

	std::cout << count << " " << Car::Cars.size() << std::endl;

*/
	/*
失败的分组算法
	std::vector<std::vector<Car>> cars_group;
	//	Divide_Group(cars, cars_group, 0);

	for (int i = 0; i < cars_group.size(); i++)
	{
		//std::cout << "Next group:" << cars_group[i].size() << std::endl;
		for (int j = 0; j < cars_group[i].size(); j++)
		{
			//	cars_group[i][j].Display();
		}
	}
	std::cout << cars_group.size() << std::endl;

*/

	std::cout << time << std::endl;

	auto comp2 = [](Car car1, Car car2) { return car1.id < car2.id; };
	std::sort(Car::Cars.begin(), Car::Cars.end(), comp2);
	WriteAnswer(Car::Cars, answerPath);

	gettimeofday(&end, NULL);
	diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	std::cout << diff << std::endl;

	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file

	return 0;
}