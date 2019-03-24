#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <cmath>
#include <set>
#include "Cross.h"
#include "Road.h"
#include "Car.h"

class Car;
class Road;
class Cross;


double Cal_difference(std::set<int> &a, std::set<int> &b);

bool Is_newgroup(double diff_value);

bool Is_Samegroup(std::set<int> &a, std::set<int> &b);

int Divide_Group(std::vector<Car> &cars, std::vector<std::vector<Car>> &cars_group, int num_of_group ); //分组。将车分成多个组，使得每个组内相关性达到一个阈值

bool IsAlmostEqual(double x, double y); //判断两个浮点数是否相等

int Get_Road_by_Two_crossid( int cross_src_id, int cross_dst_id);

int Road_findpos_by_id(int roadid); //根据Road的id找到对应在Road数组中的位置

int Cross_findpos_by_id(int crossid); //根据Cross的id找到对应在crosses数组中的位置

std::string num_trim(std::string &);
int ReadCar(std::vector<Car> &, const std::string);
int ReadRoad(std::vector<Road> &, const std::string);
int ReadCross(std::vector<Cross> &, const std::string);
int WriteAnswer(std::vector<Car> &cars, const std::string &answerPath);