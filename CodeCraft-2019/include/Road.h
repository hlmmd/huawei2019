#include <fstream>
#include <iostream>
#include <string>

#ifndef ROAD
#define ROAD
class Road
{
  public:
	static std::vector<Road> Roads;

	int id;
	int length;
	int maxspeed;
	int channel;
	int src_cross;
	int dst_cross;
	int is_dup;

	int cars_pass_num;

	//zhao
	int carCapcity;
	std::vector<std::vector<int>> forwardBucket;
	std::vector<std::vector<int>> backwardBucket;
	std::vector<std::vector<int>> *provideBucket;
	std::vector<std::vector<int>> *receiveBucket;
	int fx, fy, bx, by;
	int forwardNum, backwardNum;
	int forwardDone, backwardDone;
	int *px, *py, *provideNum, *receiveNum;
	int *provideDone;
	//end

  public:
	Road(int i, int l, int m, int c, int s, int d, int is_d)
	{
		id = i;
		length = l;
		maxspeed = m;
		channel = c;
		src_cross = s;
		dst_cross = d;
		is_dup = is_d;
		cars_pass_num = 0;
	}

	Road()
	{
	}

	int ReadRoad(const std::string);
	void Display();
	int Get_origin_priority();

	//zhao
	std::string chooseAbsoluteBucket(int crossId, std::string pr);
	void setBucket(int crossId);
	void stepInit();
	void moveInChannel(std::vector<std::vector<int>> &bucket, int channel);
	int findCar(int st, int end, int channel, std::vector<std::vector<int>> &bucket);
	int firstPriorityCar();
	int getSpeed();
	void firstPriorityCarAct(int action);
	int receiveCar(int carId);
	//end;
};

#endif