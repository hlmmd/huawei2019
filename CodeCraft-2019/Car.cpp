#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <climits> //INT_MAX

#include "common.h"

#include <unistd.h>
#include <stdlib.h>

//静态成员变量需要声明
std::vector<Car> Car::Cars ;

int Car::ReadCar(const std::string car_infostr)
{
    std::vector<std::string> res;
    std::string result;

    std::stringstream ss(car_infostr);

    while (getline(ss, result, ','))
    {
        result = num_trim(result);
        res.push_back(result);
    }

    if (res.size() != 5)
        exit(0);

    id = stoi(res[0]);
    src = stoi(res[1]);
    dst = stoi(res[2]);
    maxspeed = stoi(res[3]);
    start_time = stoi(res[4]);
    return 0;
}

void Car::Display()
{
    //   std::cout << id << " " << src << " " << dst << " " << maxspeed << " " << start_time << std::endl;

    for (int i = 0; i < road_seq.size(); i++)
        std::cout << road_seq[i] << " ";
    std::cout << std::endl;
}

int Car::getpath( std::vector<int> &path, int src_cross, int dst_cross)
{
    std::stack<int> s;
    int u = dst_cross;
    int v = dst_cross;
    while (v != src_cross)
    {
        s.push(v);
        v = path[v];
    }
    s.push(v);
    while (!s.empty())
    {
        int old = s.top();
        //  std::cout<< Crosses[ s.top()].id<<" ";
        s.pop();
        if (!s.empty())
        {
            int road_id = Get_Road_by_Two_crossid( Cross::Crosses[old].id, Cross::Crosses[s.top()].id);
            road_seq.push_back(road_id);
        }
    }
    std::set<int> st(road_seq.begin(), road_seq.end());
    road_set = st;
    //   std::cout << std::endl;
    return 0;
}

int Car::CalDijkstraPath()
{
    std::vector<double> dist(Cross::Crosses.size(), 9999999);
    std::vector<bool> visited(Cross::Crosses.size(), false);
    std::vector<int> path(Cross::Crosses.size(), -1);

    int v0 = Cross_findpos_by_id( src);
    dist[v0] = 0;
    path[v0] = v0;

    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        //判断起始点能到达哪些路口
        int pos = Cross_findpos_by_id( src);
        if (Cross::Crosses[pos].dir[i] != -1)
        {
            //如果路存在，则根据最大车速、道路限速、道路长度计算出该段距离，作为两个路口间的最短路径。
            int roadpos = Road_findpos_by_id( Cross::Crosses[pos].dir[i]);
            int roadlength = Road::Roads[roadpos].length;
            int speed = std::min(maxspeed, Road::Roads[roadpos].maxspeed); //比较车速和限速，取较小值

            //这里存在一个问题，道路长度能否整除speed?如果不能，则向上取整?
            //路可能是双向的，注意判断起点终点

            //单向路不做处理
            if (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id && Road::Roads[roadpos].is_dup == 0)
                continue;

            int dst_id = (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id) ? Road::Roads[roadpos].src_cross : Road::Roads[roadpos].dst_cross;

            int dist_pos = Cross_findpos_by_id( dst_id);

            dist[dist_pos] = ((double)roadlength / speed);
            //dist[dist_pos] = (roadlength / speed) + ((roadlength % speed) != 0);

            //       std::cout << pos << " " << roadpos << " " << dst_id << " "<<roadlength<<" "<<speed<<" "<<dist[dist_pos]<<std::endl;
            path[dist_pos] = v0;
        }
    }

    //四个路口遍历过后，置为true，表示已经得到最短路径
    visited[v0] = true;

    for (int c = 1; c < Cross::Crosses.size(); c++)
    {

        double tempmin = 9999999;
        int minpos = INT_MAX;
        int road_priority = 999999;
        // for (int i = 0; i < Cross::Crosses.size(); i++)
        // {
        //     if (visited[i] == false && dist[i] < tempmin)
        //     {
        //         std::cout << i << " " << dist[i] << std::endl;
        //     }
        // }

        for (int i = 0; i < Cross::Crosses.size(); i++)
        {
            if (visited[i] == false && dist[i] < tempmin)
            {
                tempmin = dist[i];
                minpos = i;
            }
            //如果有相同的路径，如何选择?考虑道路的拥挤情况

            else if(visited[i] == false && IsAlmostEqual(dist[i],tempmin))
            {
                

            }
        }



        // std::vector<int> Random_pos;
        // for (int i = 0; i < Cross::Crosses.size(); i++)
        // {
        //     if (visited[i] == false &&  IsAlmostEqual( dist[i],tempmin))
        //     {
        //         Random_pos.push_back(i);
        //     }
        // }

        //
        // minpos =Random_pos[ rand()%Random_pos.size()];

        if (Cross::Crosses[minpos].id == dst)
        {
            // std::cout<<"AAA"<<std::endl;
            getpath(path, Cross_findpos_by_id( src), Cross_findpos_by_id(dst));
            return dist[minpos];
        }

        //     std::cout << minpos << "AAA" << std::endl;
        //一遍循环后，得到一个最小的dist值，这个值必定对应到一个结点的最小路径

        for (int i = 0; i < ROADS_OF_CROSS; i++)
        {
            //判断起始点能到达哪些路口
            //int pos = Cross_findpos_by_id(Crosses, minpos);
            int pos = minpos;
            if (Cross::Crosses[pos].dir[i] != -1)
            {
                //如果路存在，则根据最大车速、道路限速、道路长度计算出该段距离，作为两个路口间的最短路径。
                int roadpos = Road_findpos_by_id(Cross::Crosses[pos].dir[i]);
                int roadlength = Road::Roads[roadpos].length;
                int speed = std::min(maxspeed, Road::Roads[roadpos].maxspeed); //比较车速和限速，取较小值

                //单向路不做处理
                if (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id && Road::Roads[roadpos].is_dup == 0)
                    continue;

                //这里存在一个问题，道路长度能否整除speed?如果不能，则向上取整?
                double tempdist = ((double)roadlength / speed);
                int dst_id = (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id) ? Road::Roads[roadpos].src_cross : Road::Roads[roadpos].dst_cross;

                //std::cout << pos << " " << roadpos << " " << dst_id << std::endl;
                int dist_pos = Cross_findpos_by_id( dst_id);
                if (visited[dist_pos] == false && dist[dist_pos] > (tempmin + tempdist))
                {
                    //只有比原值更小才更新

                    dist[dist_pos] = tempmin + tempdist;
                    path[dist_pos] = pos;

                    //    std::cout << pos << " " << roadpos << " " << dst_id << " "<<roadlength<<" "<<speed<<" "<<dist[dist_pos]<<std::endl;
                }
            }
        }
        //     exit(0);
        visited[minpos] = true;
    }

    return 0;
}

int Car::WriteAnswer(const std::string &answerPath)
{
    std::ofstream out;
    out.open(answerPath, std::ios::out);
}
