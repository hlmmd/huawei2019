#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include "Cross.h"
#include "Road.h"
#include "Car.h"

class Car;
class Road;
class Cross;


int Get_Road_by_Two_crossid(std::vector<Road> &roads, std::vector<Cross> &crosses, int cross_src_id, int cross_dst_id);

int Road_findpos_by_id(std::vector<Road> &roads,int roadid);//根据Road的id找到对应在Road数组中的位置

int Cross_findpos_by_id(std::vector<Cross> &crosses,int crossid); //根据Cross的id找到对应在crosses数组中的位置

std::string num_trim(std::string & );
int ReadCar( std::vector<Car> & ,const std::string );
int ReadRoad( std::vector<Road> & ,const std::string );
int ReadCross( std::vector<Cross> & ,const std::string );
int WriteAnswer(std::vector<Car> &cars, const std::string &answerPath);