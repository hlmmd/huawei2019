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
    int carCapcity;
    vector<vector<int>> forwardBucket;
    vector<vector<int>> backwardBucket;
    vector<vector<int>> provideBucket;
    vector<vector<int>> receiveBucket;
    int fx, fy, bx, by;
    int forwardNum, backwardNum;
    bool forwardDone, backwardDone;
    int px,py,provideNum, receiveNum;
    int provideDone;
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

        carCapcity = channel*length;
        for(int i=0; i<lenght; i++){
            vector<int> tmp;
            for(int j=0; j<channel;j++){
                tmp.push_back(0);
            }
            forwardBucket.push_back(tmp);
            if(is_dup)
                backwardBucket.push_back(tmp);
        }
        fx = 0, fy=0, bx =0, by=0;
        forwardNum=0, backwardNum = 0;
        forwardDone = false, backwardDone = false;
        px=-1,py=-1,provideNum=-1,receiveNum=-1;
        provideNum=-1;


    }

    Road()
    {

    }

    int ReadRoad(const std::string );
    void Display();

    
};

#endif