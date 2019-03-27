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
<<<<<<< HEAD
    vector<vector<int>> forwardBucket;
    vector<vector<int>> backwardBucket;
    vector<vector<int>> provideBucket;
    vector<vector<int>> receiveBucket;
    int fx, fy, bx, by;
    int forwardNum, backwardNum;
    bool forwardDone, backwardDone;
    int px,py,provideNum, receiveNum;
    int provideDone;
=======
    std::vector<std::vector<int>> forwardBucket;
	std::vector<std::vector<int>> backwardBucket;
	std::vector<std::vector<int>> *provideBucket;
	std::vector<std::vector<int>> *receiveBucket;
    int fx, fy, bx, by;
    int forwardNum, backwardNum;
    int forwardDone, backwardDone;
    int*px,*py,*provideNum, *receiveNum;
    int* provideDone;
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27
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
<<<<<<< HEAD

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


=======
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27
    }

    Road()
    {

    }

    int ReadRoad(const std::string );
    void Display();
<<<<<<< HEAD

=======
	std::string chooseAbsoluteBucket(int crossId, std::string pr);
	void setBucket(int crossId);
	void stepInit();
	void moveInChannel(std::vector<std::vector<int>> & bucket, int channel);
	int findCar(int st, int end, int channel, std::vector<std::vector<int>>& bucket);
	int firstPriorityCar();
	int getSpeed();
	void firstPriorityCarAct(int action);
	int receiveCar(int carId);
>>>>>>> 01b01c27dc1e3eca311561047660c63f6f8e9b27
    
};

#endif