#include <fstream>
#include <iostream>
#include <string>

#ifndef CROSS
#define CROSS

#define ROADS_OF_CROSS 4
class Road;
inline size_t key(int i, int j) { return (size_t)i << 32 | (unsigned int)j; }
class Cross
{
  public:
    static std::vector<Cross> Crosses;

    int id;
    std::vector<int> dir;
    int up;
    int right;
    int down;
    int left;

    int cars_pass_num; //所有车安排后经过该路口的总数（不包括起点）
    int road_channel_num;

    //zhao
    std::vector<int> roadIds;
    std::unordered_map<size_t, int> directionMap;
    std::vector<int> providerDirection, receiverDirection, validRoadDirecction;
    std::vector<int> provider, receiver, validRoad;

    std::unordered_map<int, std::vector<int>> carport;
    std::vector<int> readyCars, leftCars;
    int parkCarNum, finishCarNum;
    bool done, update;
    //end

    Cross(int i, int u, int r, int d, int l)
    {
        id = i;
        up = u;
        right = r;
        down = d;
        left = l;
    }
    Cross()
    {
    }

    int ReadCross(const std::string);
    void Display();
    int RemoveSingleRoad();
    int Cal_road_channel_num();

    int Cal_priority();

    //zhao
    void carportInital(int timePlane, int carId);
    void step();
    void outOfCarport();
    void setDone(bool flag);
    bool isConflict(int fromA, int directionA, int fromB, int directionB);
    //end
};

class Simulation
{
  public:
    bool dead;
    Simulation()
    {
        dead = false;
    }
    void step();
    void simulate();
};

#endif