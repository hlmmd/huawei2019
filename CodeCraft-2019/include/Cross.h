#include <fstream>
#include <iostream>
#include <string>


#ifndef CROSS
#define CROSS

#define ROADS_OF_CROSS 4
class Road;

class Cross{
public:
    int id;
    std::vector<int> dir;
    int up;
    int right;
    int down;
    int left;

    Cross(int i,int u,int r,int d,int l)
    {
       id = i;
       up = u;
       right = r;
       down=d;
       left =l;
    }
    Cross()
    {

    }

    int ReadCross(const std::string );
    void Display();
    int RemoveSingleRoad(std::vector<Road> &roads);
};

#endif