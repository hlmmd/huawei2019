#include <fstream>
#include <iostream>
#include <string>


#ifndef CROSS
#define CROSS

#define ROADS_OF_CROSS 4
class Road;

class Cross{
public:

    static std::vector<Cross> Crosses;

    int id;
    std::vector<int> dir;
    int up;
    int right;
    int down;
    int left;

    int cars_pass_num;
    int road_channel_num;
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
    int RemoveSingleRoad();
    int Cal_road_channel_num();
};

#endif