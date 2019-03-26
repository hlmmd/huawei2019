#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef CAR
#define CAR
class Car{
public:
    int id;
    int src;
    int dst;
    int maxspeed;
    int start_time, plane_time;

	int x, y, routeIndex;
    int state;
    int curRoad, nextCrossId;
    bool wait;
    std::vector<int> route;

    std::vector<int> road_seq;

public:

    Car()
    {
    }

    Car(int i,int s,int d,int m,int time)
    {
        id = i;
        src = s;
        dst = d;
        maxspeed=m;
        start_time =time;
        state = 0;
		plane_time = 0;
        curRoad = -1, nextCrossId = s;
        wait = false;
		x = 0, y = 0;
		routeIndex = 0;
    }
    int ReadCar(const std::string carPath);
    void Display();

    int CalDijkstraPath(std::vector<Cross>& Crosses,std::vector<Road>& Roads);
    int getpath(std::vector<Cross>& Crosses,std::vector<Road>& Roads,std::vector<int> &path,int src_cross ,int dst_cross);
    int WriteAnswer(const std::string & answerPath);

	void updateDynamic(int state, int x, int y, int curRoad, int roadSpeed, int nextCrossId);
    void startInit(int plane_time, std::vector<int>& rou);
	int getSpeed();
	int getNextRoad();











};

#endif