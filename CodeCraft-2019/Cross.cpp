#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

int Cross::ReadCross(const std::string cross_infostr)
{
    std::vector<std::string> res;
    std::string result;

    std::stringstream ss(cross_infostr);

    while (getline(ss, result, ','))
    {
        result = num_trim(result);
        res.push_back(result);
    }

    if (res.size() != 5)
        exit(0);

    id = stoi(res[0]);

    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        dir.push_back(stoi(res[i + 1]));
    }

    up = stoi(res[1]);
    right = stoi(res[2]);
    down = stoi(res[3]);
    left = stoi(res[4]);
    return 0;
}

int Cross::RemoveSingleRoad(std::vector<Road> &roads)
{
    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        if (dir[i] != -1)
        {
            int road_pos = Road_findpos_by_id(roads, dir[i]);
            if (roads[road_pos].src_cross != id && roads[road_pos].dst_cross == id && roads[road_pos].is_dup == 0)
                dir[i] = -1;
        }
    }
    return 0;
}

void Cross::Display()
{
    //  std::cout << id << " " << up << " " << right << " " << down << " " << left << std::endl;
    std::cout << id << " ";
    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        std::cout << dir[i] << " ";
    }
    std::cout << std::endl;
}
