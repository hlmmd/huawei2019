#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#ifndef CAR
#define CAR
class Car{
public:
    int id;
    int src;
    int dst;
    int maxspeed;
    int start_time;

    std::vector<int> road_seq;
    std::set<int> road_set;
    
    static std::vector<Car> Cars;

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
    }
    int ReadCar(const std::string carPath);
    void Display();

    int CalDijkstraPath();
    int getpath(std::vector<int> &path,int src_cross ,int dst_cross);
    int WriteAnswer(const std::string & answerPath);
};

#endif