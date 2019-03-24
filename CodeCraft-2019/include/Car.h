#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#ifndef CAR
#define CAR
class Car{
public:
    int id;
    int src;
    int dst;
    int maxspeed;
    int start_time;

    std::vector<int> road_seq;
    std::set<int> road_set;
    
    static std::vector<Car> Cars;

    //车的行驶方向。分为东北、西北、东南、西南四种方向，根据最短路径确定一个方向。
    //再根据方向重新计算最短路径，如果不存在则重新确定方向。
    
    std::vector<int> dir_seq;
    bool is_dir_type_set;
    int dir_type ;

public:

    Car()
    {
    }

    Car(int i,int s,int d,int m,int time)
    {
       id = i;
       src = s;
       dst = d;
       maxspeed=m;
       start_time =time;
    }
    int ReadCar(const std::string carPath);
    void Display();

    int CalDijkstraPath();
    int getpath(std::vector<int> &path,int src_cross ,int dst_cross);
    int WriteAnswer(const std::string & answerPath);

    int set_dir_type();
    int get_next_dir_type();
    int CalDijkstraPath_withdir();
};

#endif