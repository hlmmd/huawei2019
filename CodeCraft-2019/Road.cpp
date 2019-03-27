#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

using namespace std;
using std::vector;
using std::string;

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
//zhao
    carCapcity = channel*length;
	for (int i = 0; i<length; i++) {
		std::vector<int> tmp;
		for (int j = 0; j<channel; j++) {
			tmp.push_back(-1);
		}
		forwardBucket.push_back(tmp);
		if (is_dup)
			backwardBucket.push_back(tmp);
	}
	fx = 0, fy = 0, bx = 0, by = 0;
	forwardNum = 0, backwardNum = 0;
	forwardDone = false, backwardDone = false;
	px = NULL, py = NULL, provideNum = NULL, receiveNum = NULL;
	provideNum = NULL;
    //end
    return 0;
}

void Road::Display()
{
    std::cout << id << " " << length << " " << maxspeed << " " << channel
              << " " << src_cross << " " << dst_cross << " " << is_dup << std::endl;
}

string Road::chooseAbsoluteBucket(int crossId,string pr){
	if (crossId == this->src_cross && pr == "provide")
		return "backward";
	else if (crossId == this->src_cross && pr == "receive")
		return "forward";
	else if (crossId == this->dst_cross && pr == "provide")
		return "forward";
	else if (crossId == this->dst_cross && pr == "receive")
		return "backward";
	else
		cout << "Keywords mistake in CAR.chooseAbsoluteBucket()" << endl;
}


void Road::setBucket(int crossId){
    string bucket = chooseAbsoluteBucket(crossId, "provide");
    if(bucket == "forward"){
		provideBucket = &forwardBucket;
//        provideBucket.assign(forwardBucket.begin(), forwardBucket.end());
        px = &fx, py = &fy;
        provideDone = &forwardDone, provideNum = &forwardNum;
        if(is_dup){
			receiveBucket = &backwardBucket;
            //receiveBucket.assign(backwardBucket.begin(), backwardBucket.end());
            receiveNum = &backwardNum;
        }
        else{
            receiveBucket = NULL;
            receiveNum = NULL;
        }
    }
    else{
		receiveBucket = &forwardBucket;
        //receiveBucket.assign(forwardBucket.begin(), forwardBucket.end());
        receiveNum = &forwardNum;
        if(is_dup){
			provideBucket = &backwardBucket;
            //provideBucket.assign(backwardBucket.begin(), backwardBucket.end());
            px = &bx, py = &by;
            provideDone = &backwardDone, provideNum = &backwardNum; 
        }
        else{
            provideBucket = NULL;
            px = NULL, py = NULL, provideDone =NULL, provideNum = NULL;
        }
    }
}

void Road::stepInit(){
    fx = 0, fy = 0, bx = 0, by = 0;
    forwardDone = false, backwardDone = false;
    provideBucket = NULL;
    receiveBucket = NULL;
    px = NULL, py = NULL, provideNum = NULL, receiveNum = NULL;
    provideDone = NULL;
	for (int i = 0; i < this->length; i++) {
        for(int j=0; j<this->channel; j++){
            if(forwardBucket[i][j] != -1){
                CarDict[forwardBucket[i][j]]->updateDynamic(1,-1,-1,-1,-1,-1);
            }
            if(is_dup){
                if(backwardBucket[i][j] != -1){
                    CarDict[backwardBucket[i][j]]->updateDynamic(1, -1, -1, -1, -1, -1);
                }
            }
        }
    }

    for(int i=0; i<channel; i++){
        moveInChannel(forwardBucket, i);
        if(is_dup)
            moveInChannel(backwardBucket, i);
    }
}
// park wait finish end
void Road::moveInChannel(vector<vector<int>> & bucket, int channel){
    int previousCar = -1, previousState = 1;
    for(int i=0; i<length; i++){
        if(bucket[i][channel] != -1){
            Car* car = CarDict[bucket[i][channel]];
            int v = min(car->getSpeed(), maxspeed);
            if(car->state == 2){
                previousCar = i;
                previousState = 2;
                continue;
            }
            else if(i-v > previousCar){
                bucket[i-v][channel] = bucket[i][channel];
                bucket[i][channel] = -1;
                previousCar = i-v;
                previousState = 2;
                car->updateDynamic(2,previousCar, -1, -1, -1, -1);
            }
            else if(previousState == 2){
                if(previousCar +1 != i){
                    bucket[previousCar + 1][channel] = bucket[i][channel];
                    bucket[i][channel] = -1;
                }
                previousCar++;
                previousState = 2;
                car->updateDynamic(2,previousCar, -1, -1, -1, -1);
            }
            else{
                previousCar = i;
                previousState = 1;
            }
        }
    }
}

int Road::findCar(int st, int end, int channel, vector<vector<int>>& bucket){
    for(int i=end; i>st; i--){
        if(bucket[i][channel] != -1)
            return i;
    }
    return -1;
}

int Road::firstPriorityCar(){
    if(provideBucket->empty())
        cout << "Please do CAR.setBucket() first!" << endl;
    while(*px < this->length){
        int carId = (*provideBucket)[*px][*py];
        if(carId != -1 && CarDict[carId]->state !=2){
            Car* car = CarDict[carId];
            int left = min(car->getSpeed(), getSpeed());
            if(left > *px && findCar(-1, (*px)-1, *py, (*provideBucket)) == -1)
                return (*provideBucket)[*px][*py];
        }
        if((*py) == channel-1){
            (*px)++;
            (*py) = 0;
        }
        else{
            (*py)++;
        }
    }
    *provideDone = true;
    return -1;
}

void Road::firstPriorityCarAct(int action){
    if(provideBucket->empty())
        cout << "Please do CAR.setBucket() first!" << endl;
    if(action == 0){
        (*provideBucket)[*px][*py] = -1;
        (*provideNum)--;
    }
    else if(action == 1){
        int carId = (*provideBucket)[*px][*py];
        (*provideBucket)[*px][*py] = -1;
        (*provideBucket)[0][*py] = carId;
    }
    moveInChannel(*provideBucket,*py);
}

int Road::receiveCar(int carId){
    if(receiveBucket->empty())
        cout << "Please do CAR.setBucket() first!" << endl;
    Car* car = CarDict[carId];
	int v = car->getSpeed();
    int leftx = min(maxspeed, car->maxspeed- car->x);
    int nextCrossId = car->nextCrossId!=src_cross?src_cross:dst_cross;
    if(leftx <= 0){
        car->updateDynamic(2, 0, -1, -1, -1, -1);
        return 1;
    }
    for(int i=0; i<channel; i++){
        int frontCarLoc = findCar(length-leftx-1, length-1, i, *receiveBucket);
        if(frontCarLoc == -1){
            (*receiveBucket)[length-leftx][i] = carId;
            (*receiveNum)++;
            car->updateDynamic(2, length-leftx, i,id, maxspeed, nextCrossId);
            return 0;
        }
        Car* frontCar = CarDict[(*receiveBucket)[frontCarLoc][i]];
        if(frontCar->state == 1)
            return 2;
        else if(frontCarLoc != this->length-1){
            (*receiveBucket)[frontCarLoc+1][i] = carId;
            (*receiveNum)++;
            car->updateDynamic(2, frontCarLoc+1, i,id,maxspeed, nextCrossId);
            return 0;
        }
        else
            continue;
    }
    car->updateDynamic(2,0, -1, -1, -1, -1);
    return 1;
}




int Road::getSpeed(){
    return maxspeed;
}


