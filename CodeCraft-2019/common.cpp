#include "common.h"


//求一个数组中出现次数最多的前K个数  leetcode 347
std::vector<int> TopKFrequent(std::vector<int> &nums, int k)
{
    std::unordered_map<int, int> m;

    for (auto num : nums)
        ++m[num];
    std::vector<std::vector<int>> buckets(nums.size() + 1);
    for (auto p : m)
        buckets[p.second].push_back(p.first);

    std::vector<int> ans;
    for (int i = buckets.size() - 1; i >= 0 && ans.size() < k; --i)
    {
        for (int num : buckets[i])
        {
            ans.push_back(num);
            if (ans.size() == k)
                break;
        }
    }
    return ans;
}

int Get_next_cross_id(int cross_id, int road_id)
{
    int road_pos = Road_findpos_by_id(road_id);

    if (Road::Roads[road_pos].src_cross == cross_id)
    {
        return Road::Roads[road_pos].dst_cross;
    }
    else
    {
        return Road::Roads[road_pos].src_cross;
    }
}

double Cal_difference(std::set<int> &a, std::set<int> &b)
{

    //判断两个set的相关程度（交集/a)
    //  static double inter_value = 0.1;
    std::set<int> c;
    set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.begin()));
    // std::set<int> d;
    // set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(d, d.begin()));

    //  std::cout<<" AAAAAA "<<(double)a.size() / c.size()<<std::endl;

    return (double)a.size() / c.size();
}

bool Is_newgroup(double diff_value)
{
    //判断两个set的相关程度（交集/并集)
    const double inter_value = 0.33;
    if (inter_value > diff_value)
        return true;
    else
    {
        return false;
    }
}

bool Is_Samegroup(std::set<int> &a, std::set<int> &b)
{
    //判断两个set的相关程度（交集/并集)
    static double inter_value = 0.2;
    std::set<int> c;
    set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.begin()));
    std::set<int> d;
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(d, d.begin()));

    //   std::cout<< (double)d.size() / c.size()<<std::endl;

    if (inter_value < (double)d.size() / c.size())
        return true;
    else
    {
        return false;
    }
}

int Divide_Group(std::vector<Car> &cars, std::vector<std::vector<Car>> &cars_group, int num_of_group)
{
    if (cars.size() == 0)
        return 0;
    std::vector<Car> v0;
    cars_group.push_back(v0);
    cars_group[0].push_back(cars[0]);

    for (int i = 1; i < cars.size(); i++)
    {
        bool inserted = false;
        int maxpos = -1;
        double maxdiff = -1;
        for (int j = 0; j < cars_group.size(); j++)
        {
            double diff = 0;
            for (int k = 0; k < 10 && k < cars_group[j].size(); k++)
            {
                //     std::cout<<diff<<std::endl;
                diff += Cal_difference(cars[i].road_set, cars_group[j][rand() % cars_group[j].size()].road_set);
            }

            diff /= std::min((int)cars_group[j].size(), 10);
            //     std::cout<<diff<<std::endl;

            if (Is_newgroup(diff))
            {
                std::cout << "new group" << std::endl;
                inserted = true;
                cars_group.push_back(v0);
                cars_group[cars_group.size() - 1].push_back(cars[i]);
                break;
            }
            if (maxdiff < diff)
            {
                maxdiff = diff;
                maxpos = j;
            }

            // if (Is_Samegroup(cars[i].road_set, cars_group[j][0].road_set))
            // {
            //     inserted = true;
            //     cars_group[j].push_back(cars[i]);
            //     break;
            // }
        }
        if (inserted == false)
        {
            cars_group[maxpos].push_back(cars[i]);
        }

        // if(inserted==false)//没有找到同类，则新开一个组
        // {
        //  //   std::cout<<"new"<<std::endl;
        //     cars_group.push_back(v0);
        //     cars_group[cars_group.size()-1].push_back(cars[i]);
        // }
    }

    return 0;
}

bool IsAlmostEqual(double x, double y)
{
    if (fabs(x - y) < 1e-5)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Get_Road_by_Two_crossid(int cross_src_id, int cross_dst_id)
{
    int pos = Cross_findpos_by_id(cross_src_id);

    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        int road_id = Cross::Crosses[pos].dir[i];
        if (road_id != -1)
        {
            int road_pos = Road_findpos_by_id(road_id);
            if (Road::Roads[road_pos].dst_cross == cross_dst_id || Road::Roads[road_pos].is_dup == 1 && Road::Roads[road_pos].src_cross == cross_dst_id)
                return road_id;
        }
    }

    std::cout << std::endl
              << cross_src_id << " " << cross_dst_id << "Get_Road_by_Two_crossid error" << std::endl;
    return 0;
}

int Road_findpos_by_id(int roadid)
{
    if (roadid == -1)
        return -1;
    static std::unordered_map<int, int> map; //使用unordered_map存储
    if (map.empty())
    {
        for (int i = 0; i < Road::Roads.size(); i++)
            map[Road::Roads[i].id] = i;
    }
    return map[roadid];
}

int Cross_findpos_by_id(int crossid)
{
    if (crossid == -1)
        return -1;
    static std::unordered_map<int, int> map; //使用unordered_map存储
    if (map.empty())
    {
        for (int i = 0; i < Cross::Crosses.size(); i++)
            map[Cross::Crosses[i].id] = i;
    }
    return map[crossid];
}

std::string num_trim(std::string &str)
{
    if (str == "")
        return "";
    int start = 0;
    while (str[start] != '-' && (str[start] < '0' || str[start] > '9'))
        start++;
    int end = str.length() - 1;
    while (str[end] < '0' || str[end] > '9')
        end--;
    return str.substr(start, end - start + 1);
}

int ReadCar(std::vector<Car> &cars, const std::string carPath)
{
    std::ifstream fin(carPath, std::ios::in);
    if (fin)
    {
        std::string line;
        getline(fin, line);
        while (getline(fin, line))
        {
            Car car;
            car.ReadCar(line);
            cars.push_back(car);
        }
        return 0;
    }
    else
    {
        std::cout << "open error" << std::endl;
        return -1;
    }
}

int ReadRoad(std::vector<Road> &roads, const std::string RoadPath)
{
    std::ifstream fin(RoadPath, std::ios::in);
    if (fin)
    {
        std::string line;
        getline(fin, line);
        while (getline(fin, line))
        {
            Road road;
            road.ReadRoad(line);
            roads.push_back(road);
        }
        return 0;
    }
    else
    {
        std::cout << "open error" << std::endl;
        return -1;
    }
}

int ReadCross(std::vector<Cross> &Crosses, const std::string CrossPath)
{
    std::ifstream fin(CrossPath, std::ios::in);
    if (fin)
    {
        std::string line;
        getline(fin, line);
        while (getline(fin, line))
        {
            Cross cross;
            cross.ReadCross(line);
            Crosses.push_back(cross);
        }
        return 0;
    }
    else
    {
        std::cout << "open error" << std::endl;
        return -1;
    }
}

int WriteAnswer(std::vector<Car> &cars, const std::string &answerPath)
{
    std::ofstream out;
    out.open(answerPath, std::ios::out);
    if (!out)
        return -1;

    for (auto car : cars)
    {
        out << '(';
        out << car.id << ',';
        out << car.start_time;
        for (auto road_id : car.road_seq)
            out << ',' << road_id;

        out << ')' << std::endl;
    }

    return 0;
}