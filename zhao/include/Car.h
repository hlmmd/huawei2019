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
<<<<<<< HEAD
    int start_time;

    int state;
    int x,y;
    int curRoad, nextCrossId;
    int deltaX, delatY;
    bool wait;
    vector<int> route;
=======
    int start_time, plane_time;

	int x, y, routeIndex;
    int state;
    int curRoad, nextCrossId;
    bool wait;
    std::vector<int> route;
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27

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
<<<<<<< HEAD
        x = 0, y = 0;
        curRoad = -1, nextCrossId = s;
        deltaX = 0, delatY = 0;
        wait = false;
=======
		plane_time = 0;
        curRoad = -1, nextCrossId = s;
        wait = false;
		x = 0, y = 0;
		routeIndex = 0;
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27
    }
    int ReadCar(const std::string carPath);
    void Display();

    int CalDijkstraPath(std::vector<Cross>& Crosses,std::vector<Road>& Roads);
    int getpath(std::vector<Cross>& Crosses,std::vector<Road>& Roads,std::vector<int> &path,int src_cross ,int dst_cross);
    int WriteAnswer(const std::string & answerPath);

<<<<<<< HEAD

    void startInit(vector<int> rou);

=======
	void updateDynamic(int state, int x, int y, int curRoad, int roadSpeed, int nextCrossId);
    void startInit(int plane_time, std::vector<int>& rou);
	int getSpeed();
	int getNextRoad();
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27











};

#endif