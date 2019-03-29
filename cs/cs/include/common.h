
#include <sstream>
#include <climits> //INT_MAX
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <cmath>
#include <set>
#include "Cross.h"
#include "Road.h"
#include "Car.h"
#include "optimize.h"
//cs paras
#define para_v1 0.02
#define para_v2 10
#define para_v3 1
#define para_choose 0.85
#define para_out 0.15          //double
//cs paras


class Car;
class Road;
class Cross;

extern std::unordered_map<int, Car*> CarDict;
extern std::unordered_map<int, Road*> RoadDict;
extern std::unordered_map<int, Cross*> CrossDict;
extern std::vector<int> CarNameSpace, RoadNameSpace, CrossNameSpace;
extern int CarDistribution[3];
extern int Time;


int Divide_speed_Group(std::vector<Car> &cars_group, std::vector<std::vector<Car>> &cars_speed_group ,std::vector<int> &car_speed);

bool finish_start_group(std::vector<Car> &cars);

//求一个数组中出现次数最多的前K个数  leetcode 347
std::vector<int> TopKFrequent(std::vector<int> &nums, int k);

int Divide_Group(std::vector<std::vector<Car>> &cars_group);

bool IsAlmostEqual(double x, double y); //判断两个浮点数是否相等

//给定当前的cross和道路，求出通往的cross的id。
int Get_next_cross_id(int cross_id, int road_id);

int Get_Road_by_Two_crossid(int cross_src_id, int cross_dst_id);

int Car_findpos_by_id(int carid); //根据Road的id找到对应在Road数组中的位置

int Road_findpos_by_id(int roadid); //根据Road的id找到对应在Road数组中的位置

int Cross_findpos_by_id(int crossid); //根据Cross的id找到对应在crosses数组中的位置

std::string num_trim(std::string &);
int ReadCar(std::vector<Car> &, const std::string);
int ReadRoad(std::vector<Road> &, const std::string);
int ReadCross(std::vector<Cross> &, const std::string);
int WriteAnswer(std::vector<Car> &cars, const std::string &answerPath);

