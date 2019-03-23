#include <fstream>
#include <iostream>
#include <string>

#ifndef ROAD
#define ROAD
class Road{
public:
    int id;
    int length;
    int maxspeed;
    int channel;
    int src_cross;
    int dst_cross;
    int is_dup;
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
    }

    Road()
    {

    }

    int ReadRoad(const std::string );
    void Display();

    
};

#endif