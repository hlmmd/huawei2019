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

	for (auto &cross : Cross::Crosses)
		cross.RemoveSingleRoad();

	int time = 1;

	auto comp = [](Car car1, Car car2) { return car1.start_time < car2.start_time; };
	std::sort(Car::Cars.begin(), Car::Cars.end(), comp);
	for (auto &car : Car::Cars)
	{
		//	car.Display();
		car.start_time = time;
		time += car.CalDijkstraPath();
		//	std::cout<<time<<" ";
		//		car.Display();
		//std::cout<<car.road_seq.size()<<" ";
	}

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
	std::cout<<cars_group.size()<<std::endl;
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