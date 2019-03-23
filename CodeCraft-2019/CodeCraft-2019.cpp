#include "iostream"
#include <vector>
#include "common.h"

int main(int argc, char *argv[])
{
	std::cout << "Begin" << std::endl;

	if (argc < 5)
	{
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);

	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;


	std::vector<Car> cars;
	ReadCar(cars, carPath);

	std::vector<Road> roads;
	ReadRoad(roads, roadPath);


	std::vector<Cross> crosses;
	ReadCross(crosses, crossPath);

	for(auto cross:crosses)
		cross.RemoveSingleRoad(roads);
	
	int time = 0;

	auto comp = [](Car car1,Car car2){return car1.start_time<car2.start_time;};
	std::sort(cars.begin(),cars.end(),comp);
	for(auto &car :cars)
	{
	//	car.Display();
		
	car.start_time+=time;
	time+=car.CalDijkstraPath(crosses,roads);
//	std::cout<<time<<" ";
//		car.Display();
//std::cout<<car.road_seq.size()<<" ";
	}

	auto comp2 = [](Car car1,Car car2){return car1.id<car2.id;};
	std::sort(cars.begin(),cars.end(),comp2);
	WriteAnswer(cars,answerPath);

	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file

	return 0;
}