#include <fstream>
#include <iostream>
#include <string>

#ifndef ROAD
#define ROAD
class Road{
public:

    static std::vector<Road> Roads;

    int id;
    int length;
    int maxspeed;
    int channel;
    int src_cross;
    int dst_cross;
    int is_dup;

    int cars_pass_num;

public:
    Road(int i,int l,int m,int c,int s,int d,int is_d)
    {
       id = i;
       length = l;
       maxspeed =m;
       channel=c;
       src_cross=s;
       dst_cross=d; 
       is_dup=is_d;
       cars_pass_num=0;
    }

    Road()
    {

    }

    int ReadRoad(const std::string );
    void Display();
    int Get_origin_priority();
    
};

#endif