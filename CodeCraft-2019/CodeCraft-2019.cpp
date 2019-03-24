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
	for (auto &cross : Cross::Crosses)
		cross.Cal_road_channel_num();

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
		time += car.CalDijkstraPath();
		car.set_dir_type();
		//	std::cout<<time<<" ";
		//		car.Display();
		//std::cout<<car.road_seq.size()<<" ";
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