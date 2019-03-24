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
    int start_time;

    int state;
    int x,y;
    int curRoad, nextCrossId;
    int deltaX, delatY;
    bool wait;
    vector<int> route;

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
        x = 0, y = 0;
        curRoad = -1, nextCrossId = s;
        deltaX = 0, delatY = 0;
        wait = false;
    }
    int ReadCar(const std::string carPath);
    void Display();

    int CalDijkstraPath(std::vector<Cross>& Crosses,std::vector<Road>& Roads);
    int getpath(std::vector<Cross>& Crosses,std::vector<Road>& Roads,std::vector<int> &path,int src_cross ,int dst_cross);
    int WriteAnswer(const std::string & answerPath);


    void startInit(vector<int> rou);












};

#endif