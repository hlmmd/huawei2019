#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

std::vector<std::vector<std::vector<double>>> Optimize::dist_time;
std::vector<std::vector<double>> Optimize::scores;
std::vector<std::vector<std::vector<std::vector<int>>>> Optimize::dist_table;
std::unordered_map<int, int> Optimize::speed_map;
std::unordered_set<int> Optimize::speed_set;

double Optimize::get_score_road(int roadid, int crossid, int carid)
{
    double distance_forward = 0;
    double speed_forward = 9999999;
    bool issrc = false;
    bool nocar = true;
    double ret = 0;
    double time_self;
    int place_num = 0;
    int lastcar_locate = 0;
    bool isfind = false;
    Road *_road = RoadDict[roadid];
    if (crossid == _road->src_cross)
    {
        issrc = true;
    }
    else
    {
        issrc = false;
    }
    if (issrc)
    {
        if (_road->forwardNum)
        {
            for (place_num = 0; place_num < _road->forwardBucket.size(); place_num++)
            {
                if (_road->forwardBucket[place_num][0] == -1)
                {
                    break;
                }
            }
            if (place_num == _road->forwardBucket.size())
            {
                ret = para_out + 1; //can't place
                return ret;
            }
            for (lastcar_locate = 0; lastcar_locate < _road->forwardBucket[place_num].size(); lastcar_locate++)
            {
                if (_road->forwardBucket[place_num][lastcar_locate] != -1)
                {
                    distance_forward = (distance_forward == 0) ? lastcar_locate : distance_forward;
                    speed_forward = fmin(CarDict[_road->forwardBucket[place_num][lastcar_locate]]->maxspeed, speed_forward);
                    speed_forward = fmin(speed_forward, _road->maxspeed);
                    nocar = false;
                    isfind = true;
                }
                if (_road->forwardBucket[place_num][lastcar_locate] == -1 && isfind)
                {
                    break;
                }
            }
            if (nocar)
            {
                ret = 0;
                return ret;
            }
            else
            {
                time_self = _road->length / fmin(_road->maxspeed, CarDict[carid]->maxspeed);
                ret = time_self - (_road->length - distance_forward) / speed_forward;
                return ret;
            }
        }
        else
        {
            ret = 0;
            return ret;
        }
    }
    else
    {
        if (_road->backwardNum)
        {
            for (place_num = 0; place_num < _road->backwardBucket.size(); place_num++)
            {
                if (_road->backwardBucket[place_num][0] == -1)
                {
                    break;
                }
            }
            if (place_num == _road->backwardBucket.size())
            {
                ret = para_out + 1; //can't place
                return ret;
            }
            for (lastcar_locate = 0; lastcar_locate < _road->backwardBucket[place_num].size(); lastcar_locate++)
            {
                if (_road->backwardBucket[place_num][lastcar_locate] != -1)
                {
                    distance_forward = (distance_forward == 0) ? lastcar_locate : distance_forward;
                    speed_forward = fmin(CarDict[_road->backwardBucket[place_num][lastcar_locate]]->maxspeed, speed_forward);
                    speed_forward = fmin(speed_forward, _road->maxspeed);
                    nocar = false;
                    isfind = true;
                }
                if (_road->backwardBucket[place_num][lastcar_locate] == -1 && isfind)
                {
                    break;
                }
            }
            if (nocar)
            {
                ret = 0;
                return ret;
            }
            else
            {
                time_self = _road->length / fmin(_road->maxspeed, CarDict[carid]->maxspeed);
                ret = time_self - (_road->length - distance_forward) / speed_forward;
                return ret;
            }
        }
        else
        {
            ret = 0;
            return ret;
        }
    }
    return ret;
}

void Optimize::normalize(std::vector<double> &value)
{
    double ymax = 1;
    double ymin = 0;
    double dMaxValue = *max_element(value.begin(), value.end());
    double dMinValue = *min_element(value.begin(), value.end());
    for (int f = 0; f < value.size(); ++f)
    {
        value[f] = (ymax - ymin) * (value[f] - dMinValue) / (dMaxValue - dMinValue + 1e-8) + ymin;
    }
    return;
}

int Optimize::update_scores(int time_sche)
{
    for (int row = 0; row < Optimize::scores.size(); row++)
    {
        for (int column = 0; column < Optimize::scores[0].size(); column++)
        {
            Optimize::scores[row][column] *= 0.9;
        }
    }
    int road_pos, last_score, dst_pos;
    for (auto car : Car::Cars)
    {
        for (auto i : car.road_seq)
        {
            road_pos = Road_findpos_by_id(i);
            dst_pos = Cross_findpos_by_id(car.dst);
            last_score = Optimize::scores[road_pos][dst_pos];
            Optimize::scores[road_pos][dst_pos] += time_sche * 0.01; /////?
        }
    }
    double sum = 0;
    for (int row = 0; row < Optimize::scores.size(); row++)
    {
        for (int column = 0; column < Optimize::scores[0].size(); column++)
        {
            sum += Optimize::scores[row][column];
        }
    }
    return 0;
}

bool Optimize::random_change()
{
    //srand(time(0));
    int rands = int(1.0 / (1.0 - para_choose));
    int k = rand() % (rands);
    if (!k)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Optimize::sort_ops(std::vector<int> &choose, std::vector<double> &value)
{
    //std::vector<double>softmax(value.size(),0);
    double sumv = 0;
    double maxv = 0;
    for (int i = 0; i < value.size(); i++)
    {
        if (value[i] > maxv)
        {
            maxv = value[i];
        }
    }
    for (int i = 0; i < value.size(); i++)
    {
        sumv += value[i];
        value[i] = exp(value[i] - maxv);
    }
    for (int i = 0; i < value.size(); i++)
    {
        value[i] /= sumv;
    }
    for (int i = 0; i < value.size(); i++)
    {
        for (int j = 1; j < value.size() - i; j++)
        {
            if (value[j - 1] < value[j])
            {
                std::swap(value[j - 1], value[j]);
                std::swap(choose[j - 1], choose[j]);
            }
        }
    }
}
//cs funcs