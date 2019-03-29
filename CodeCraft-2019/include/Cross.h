#include <fstream>
#include <iostream>
#include <string>


#ifndef CROSS
#define CROSS

#define ROADS_OF_CROSS 4
class Road;

inline size_t key(int i, int j) { return (size_t)i << 32 | (unsigned int)j; }

class Cross{
public:
    int id;
    std::vector<int> dir;
    int up;
    int right;
    int down;
    int left;
    std::vector<int> roadIds;
    std::unordered_map<size_t, int> directionMap;
    std::vector<int> providerDirection, receiverDirection, validRoadDirecction;
    std::vector<int> provider, receiver, validRoad;

    std::unordered_map<int, std::vector<int>> carport;
    std::vector<int> readyCars, leftCars;
    int parkCarNum, finishCarNum;
    bool done, update;

	Cross(int i, int u, int r, int d, int l);
    Cross()
    {

    }

    int ReadCross(const std::string );
    void Display();
    int RemoveSingleRoad(std::vector<Road> &roads);
	void carportInital(int timePlane, int carId);
	void step();
	void outOfCarport();
	void setDone(bool flag);
	bool isConflict(int fromA, int directionA, int fromB, int directionB);
	void init();
};


class Simulation{
public:
    bool dead;
	Simulation(){
        dead = false;
    }
    void step();
    void simulate();
	void init();
};




#endif