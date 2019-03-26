#include <iostream>
#include <vector>
#include "include\common.h"
#include <sstream>
using namespace std;



int main(int argc, char *argv[])
{

	std::string carPath = "./config/Car.txt";
	std::string roadPath = "./config/Road.txt";
	std::string crossPath = "./config/Cross.txt";
	std::string answerPath = "./config/answer.txt";


	std::vector<Car> cars;
	ReadCar(cars, carPath);
	for(int i=0; i<cars.size(); i++){
		CarNameSpace.push_back(cars[i].id);
		CarDict[cars[i].id] = &cars[i];
	}

	std::vector<Road> roads;
	ReadRoad(roads, roadPath);

	for (int i = 0; i<roads.size(); i++) {
		RoadNameSpace.push_back(roads[i].id);
		RoadDict[roads[i].id] = &roads[i];
	}


	std::vector<Cross> crosses;
	ReadCross(crosses, crossPath);

	for(int i=0; i<crosses.size(); i++){
		CrossNameSpace.push_back(crosses[i].id);
		CrossDict[crosses[i].id] = &crosses[i];
	}

	std::ifstream fin(answerPath, std::ios::in);
	if (fin){
		std::string line;
		while (getline(fin, line)){
			vector<std::string> res;
			string result;
			stringstream ss(line);
			while (getline(ss, result, ',')){
				result = num_trim(result);
				res.push_back(result);
			}

			int carId = stoi(res[0]);
			int planTime = stoi(res[1]);
			vector<int> route;
			for(int i=2; i<res.size(); i++){
				route.push_back(stoi(res[i]));
			}
			CarDict[carId]->startInit(planTime, route);
		}
	}
	else{
		std::cout << "open error" << std::endl;
		return -1;
	}
	CarDistribution[0] = CarNameSpace.size();
	for(int carId : CarNameSpace){
		CrossDict[CarDict[carId]->src]->carportInital(CarDict[carId]->plane_time, carId);
	}
	sort(CarNameSpace.begin(), CarNameSpace.end());
	sort(CrossNameSpace.begin(), CrossNameSpace.end());

	Simulation sim;
	sim.simulate();


	return 0;
}