#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

//静态成员变量需要声明
std::vector<Road> Road::Roads ;

int Road::Get_origin_priority()
{
    return cars_pass_num/channel;
}

int Road::ReadRoad(const std::string road_infostr)
{
    std::vector<std::string> res;
    std::string result;

    std::stringstream ss(road_infostr);

    while (getline(ss, result, ','))
    {
        result = num_trim(result);
        res.push_back(result);
    }

    if (res.size() != 7)
        exit(0);

    id = stoi(res[0]);
    length = stoi(res[1]);
    maxspeed = stoi(res[2]);
    channel = stoi(res[3]);
    src_cross = stoi(res[4]);
    dst_cross = stoi(res[5]);
    is_dup = stoi(res[6]);
    cars_pass_num=0;
    return 0;
}

void Road::Display()
{
    std::cout << id << " " << length << " " << maxspeed << " " << channel
              << " " << src_cross << " " << dst_cross << " " << is_dup << std::endl;
}
