#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <cmath>
#include "Cross.h"
#include "Road.h"
#include "Car.h"

#ifndef COMMOM_H
#define COMMOM_H

class Car;
class Road;
class Cross;

extern std::unordered_map<int, Car*> CarDict;
extern std::unordered_map<int, Road*> RoadDict;
extern std::unordered_map<int, Cross*> CrossDict;
extern std::vector<int> CarNameSpace, RoadNameSpace, CrossNameSpace;
extern int CarDistribution[3];
extern int Time;


int Divide_Group(std::vector<Car> &cars, std::vector<std::vector<Car>> &cars_group, int num_of_group); //分组。将车分成多个组，使得每个组内相关性达到一个阈值

bool IsAlmostEqual(double x, double y); //判断两个浮点数是否相等

int Get_Road_by_Two_crossid(std::vector<Road> &roads, std::vector<Cross> &crosses, int cross_src_id, int cross_dst_id);

int Road_findpos_by_id(std::vector<Road> &roads, int roadid); //根据Road的id找到对应在Road数组中的位置

int Cross_findpos_by_id(std::vector<Cross> &crosses, int crossid);


std::string num_trim(std::string &);
int ReadCar(std::vector<Car> &, const std::string);
int ReadRoad(std::vector<Road> &, const std::string);
int ReadCross(std::vector<Cross> &, const std::string);
int ReadAnswer(const std::string);
int WriteAnswer(std::vector<Car> &cars, const std::string &answerPath);

#endif