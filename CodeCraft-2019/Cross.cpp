#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

using std::unordered_map;
using std::vector;
using namespace std;
//静态成员变量需要声明
std::vector<Cross> Cross::Crosses;

void Cross::init()
{

    readyCars.clear(), leftCars.clear();
    carport.clear();

    parkCarNum = 0;
    finishCarNum = 0;
    done = false;
    update = false;
}

void Simulation::init()
{
    dead = false;
    Time = 0;
    for (int i = 0; i < 3; i++)
        CarDistribution[i] = 0;
    for (int carId : CarNameSpace)
    {
        CarDict[carId]->init();
    }

    for (int roadId : RoadNameSpace)
    {
        RoadDict[roadId]->init();
    }

    for (int crossId : CrossNameSpace)
    {
        CrossDict[crossId]->init();
    }

    for (int carId : CarNameSpace)
    {
        Car *car = CarDict[carId];
        vector<int> new_road;
        new_road.assign(car->route.begin(), car->route.end()); //
        car->startInit(car->plane_time, new_road);             //
    }

    CarDistribution[0] = CarNameSpace.size();
    for (int carId : CarNameSpace)
    {
        //		cout << CarDict[carId]->plane_time << endl;
        CrossDict[CarDict[carId]->src]->carportInital(CarDict[carId]->plane_time, carId); //
    }
}

//计算路口遍历的优先级（当两个路口对应的最短路径相同时，如何选择？)
int Cross::Cal_priority()
{
    //除以能转向的道路个数
    int n = 0;
    for (int i = 0; i < dir.size(); i++)
        if (dir[i] != -1)
            n++;
    if (n == 0)
        exit(0);
    int r = road_channel_num / n;
    return cars_pass_num / r;
}

//计算路口车道总数
int Cross::Cal_road_channel_num()
{
    int count = 0;
    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        if (dir[i] != -1)
        {
            //双向路算作两倍的车道
            count += Road::Roads[Road_findpos_by_id(dir[i])].channel << Road::Roads[Road_findpos_by_id(dir[i])].is_dup;
        }
    }
    road_channel_num = count;
    return count;
}

int Cross::ReadCross(const std::string cross_infostr)
{
    std::vector<std::string> res;
    std::string result;

    std::stringstream ss(cross_infostr);

    while (getline(ss, result, ','))
    {
        result = num_trim(result);
        res.push_back(result);
    }

    if (res.size() != 5)
        exit(0);

    id = stoi(res[0]);

    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        dir.push_back(stoi(res[i + 1]));
    }

    up = stoi(res[1]);
    right = stoi(res[2]);
    down = stoi(res[3]);
    left = stoi(res[4]);

    cars_pass_num = 0;

    //zhao
    roadIds.push_back(up);
    roadIds.push_back(right);
    roadIds.push_back(down);
    roadIds.push_back(left);
    directionMap[key(up, right)] = 1;
    directionMap[key(up, down)] = 2;
    directionMap[key(up, left)] = -1;

    directionMap[key(right, down)] = 1;
    directionMap[key(right, left)] = 2;
    directionMap[key(right, up)] = -1;

    directionMap[key(down, left)] = 1;
    directionMap[key(down, up)] = 2;
    directionMap[key(down, right)] = -1;

    directionMap[key(left, up)] = 1;
    directionMap[key(left, right)] = 2;
    directionMap[key(left, down)] = -1;
    unordered_map<int, int> tmp_provider;
    //cout<<"1"<<endl;
    for (int i = 0; i < roadIds.size(); i++)
    {
        if (roadIds[i] == -1)
            continue;
        Road *road = RoadDict[roadIds[i]];
        if (road->is_dup || road->dst_cross == this->id)
        {
            provider.push_back(roadIds[i]);
            tmp_provider[roadIds[i]] = i;
        }
        if (road->is_dup || road->src_cross == this->id)
        {
            receiverDirection.push_back(i);
            receiver.push_back(roadIds[i]);
        }
        validRoadDirecction.push_back(i);
        validRoad.push_back(roadIds[i]);
    }
    //cout<<"1"<<endl;
    sort(provider.begin(), provider.end());
    for (int t : provider)
    {
        providerDirection.push_back(tmp_provider[t]);
    }

    parkCarNum = 0;
    finishCarNum = 0;
    done = false;
    update = false;

    //end

    return 0;
}

int Cross::RemoveSingleRoad()
{
    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        if (dir[i] != -1)
        {
            int road_pos = Road_findpos_by_id(dir[i]);
            if (Road::Roads[road_pos].src_cross != id && Road::Roads[road_pos].dst_cross == id && Road::Roads[road_pos].is_dup == 0)
                dir[i] = -1;
        }
    }
    return 0;
}

void Cross::Display()
{
    //  std::cout << id << " " << up << " " << right << " " << down << " " << left << std::endl;
    std::cout << id << " ";
    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        std::cout << dir[i] << " ";
    }
    std::cout << std::endl;
}

//zhao
void Cross::step()
{
    update = false;
    for (int roadId : validRoad)
    {
        RoadDict[roadId]->setBucket(id);
    }
    vector<int> nextCarId, nextRoad, nextDirection;
    vector<Car *> nextCar;

    for (int i = 0; i < provider.size(); i++)
    {
        nextCarId.push_back(RoadDict[provider[i]]->firstPriorityCar());
        if (nextCarId[i] != -1)
        {
            nextCar.push_back(CarDict[nextCarId[i]]);
            nextRoad.push_back(nextCar[i]->getNextRoad());
            if (nextRoad[i] == -1)
                nextDirection.push_back(2);
            else
                nextDirection.push_back(directionMap[key(provider[i], nextRoad[i])]);
        }
        else
        {
            nextCar.push_back(NULL);
            nextRoad.push_back(-1);
            nextDirection.push_back(-1);
        }
    }
    for (int i = 0; i < provider.size(); i++)
    {
        while (nextCar[i] != NULL)
        {

            Road *prov = RoadDict[this->provider[i]];

            for (int j = 0; i < this->provider.size(); j++)
            {
                if (nextCar[j] != NULL && isConflict(this->providerDirection[i], nextDirection[i], this->providerDirection[j], nextDirection[j]))
                    ;
                break;
            }
            if (nextRoad[i] == -1)
            {
                prov->firstPriorityCarAct(0);
                CarDistribution[1]--;
                CarDistribution[2]++;
                this->finishCarNum++;
                this->update = true;
            }
            else
            {
                int t = nextRoad[i];
                Road *nextroad = RoadDict[nextRoad[i]];
                int action = nextroad->receiveCar(nextCar[i]->id);
                if (action == 2)
                    break;
                this->update = true;
                prov->firstPriorityCarAct(action);
            }
            nextCarId[i] = prov->firstPriorityCar();
            if (nextCarId[i] != -1)
            {
                nextCar[i] = CarDict[nextCarId[i]];
                nextRoad[i] = nextCar[i]->getNextRoad();
                if (nextRoad[i] == -1)
                    nextDirection[i] = 2;
                else
                    nextDirection[i] = directionMap[key(provider[i], nextRoad[i])];
            }
            else
            {
                nextCar[i] = NULL;
                nextRoad[i] = -1;
                nextDirection[i] = -1;
            }
        }
    }
    bool done = true;
    for (int i = 0; i < this->provider.size(); i++)
        if (nextCar[i] != NULL)
            done = false;
    this->done = done;
}

void Cross::outOfCarport()
{
    static int delays = 0;
    readyCars.assign(leftCars.begin(), leftCars.end());
    leftCars.clear();
    Optimize optimizer;
    if (carport.find(Time) != carport.end())
    {
        sort(carport[Time].begin(), carport[Time].end());
        readyCars.insert(readyCars.end(), carport[Time].begin(), carport[Time].end());
    }
    for (int i = 0; i < readyCars.size();)
    {
        double score = optimizer.get_score_road(CarDict[readyCars[i]]->route[0], CarDict[readyCars[i]]->src, readyCars[i]);
        if (score < para_out || score == para_out + 1)
        {
            i++;
        }
        else
        {
            leftCars.insert(leftCars.end(), readyCars[i]);
            readyCars.erase(readyCars.begin() + i); //delay
            delays++;
        }
    }
    //if(delays>=1000){
    //    cout<<"delay1000"<<endl;
    //}
    if (readyCars.empty())
        return;
    sort(readyCars.begin(), readyCars.end());
    for (int roadId : this->receiver)
        RoadDict[roadId]->setBucket(id);
    for (int i = 0; i < readyCars.size(); i++)
    {
        int carId = readyCars[i];
        Car *car = CarDict[carId];
        int roadId = CarDict[carId]->getNextRoad();
        Road *road = RoadDict[roadId];
        int act = road->receiveCar(carId);
        if (act != 0)
            leftCars.push_back(readyCars[i]);
        else
        {
            this->parkCarNum--;
            CarDistribution[0]--;
            CarDistribution[1]++;
        }
    }
}

void Cross::carportInital(int timePlane, int carId)
{
    if (carport.find(timePlane) == carport.end())
    {
        vector<int> tmp;
        tmp.push_back(carId);
        carport[timePlane] = tmp;
    }
    else
    {
        carport[timePlane].push_back(carId);
    }
    parkCarNum++;
}

void Cross::setDone(bool flag)
{
    this->done = flag;
}

bool Cross::isConflict(int fromA, int directionA, int fromB, int directionB)
{
    return (fromA + directionA) % 4 == (fromB + directionB) % 4 && directionA < directionB;
}

int Simulation::step()
{
    //if(Time%100==0) {
    //    std::cout << "time: " << Time << std::endl;
    //}
    for (int crossId : CrossNameSpace)
        CrossDict[crossId]->setDone(false);
    for (int road : RoadNameSpace)
        RoadDict[road]->stepInit();
    vector<int> unfinishedCross = CrossNameSpace;
    while (!unfinishedCross.empty())
    {
        this->dead = true;
        vector<int> nextCross;
        for (int crossId : unfinishedCross)
        {
            Cross *cross = CrossDict[crossId];
            cross->step();
            if (!cross->done)
                nextCross.push_back(crossId);
            if (cross->update || cross->done)
                this->dead = false;
        }
        unfinishedCross.clear();
        unfinishedCross = nextCross;
        if (this->dead == true)
        {
            cout << "dead lock : " << Time << endl;
            return -1;
        }
    }
    for (int crossId : CrossNameSpace)
    {
        for (int roadId : CrossDict[crossId]->validRoad)
        {
            RoadDict[roadId]->setBucket(crossId);
        }
        CrossDict[crossId]->outOfCarport();
    }
    return 0;
}

int Simulation::simulate()
{
    while (1)
    {
        int ret = this->step();
        if (ret == -1)
        {
            return -1;
        }
        if (CarDistribution[2] == CarNameSpace.size())
        {
            //cout << "cars:"<<CarDistribution[2] << endl;
            return Time;
        }
        if (dead)
        {
            cout << "dead" << endl;
            return -1;
        }
        Time++;
    }
    return 0;
}

//end