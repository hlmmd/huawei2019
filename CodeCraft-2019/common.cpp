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

bool finish_start_group(std::vector<Car> &cars)
{
    for (int i = 0; i < cars.size(); i++)
        if (cars[i].started == false)
            return false;
    return true;
}

int Divide_speed_Group(std::vector<Car> &cars_group, std::vector<std::vector<Car>> &cars_speed_group, std::vector<int> &car_speed)
{
    std::vector<Car> empty;
    for (int i = 0; i < car_speed.size(); i++)
        cars_speed_group.push_back(empty);

    for (auto car : cars_group)
    {
        for (int i = 0; i < car_speed.size(); i++)
        {
            if (car_speed[i] == car.maxspeed)
            {
                cars_speed_group[i].push_back(car);
                break;
            }
        }
    }

    for (auto &groups : cars_speed_group)
    {
        auto comp = [](Car car1, Car car2) {
            // if (car1.is_dir_type_set == car2.is_dir_type_set)
            //     return car1.dj_time < car2.dj_time;
            return car1.is_dir_type_set < car2.is_dir_type_set;
            //return car1.maxspeed > car2.maxspeed;
        };

        std::sort(groups.begin(), groups.end(), comp);
    }

    return 0;

    // for (auto car : cars_group)
    // {
    //     int i = 0;
    //     for (; i < cars_speed_group.size(); i++)
    //     {
    //         if (cars_speed_group[i][0].maxspeed == car.maxspeed)
    //         {
    //             cars_speed_group[i].push_back(car);
    //             break;
    //         }
    //     }

    //     if (i == cars_speed_group.size())
    //     {
    //         cars_speed_group.push_back(empty);
    //         cars_speed_group[i].push_back(car);
    //     }
    // }

    // for(int i = 0 ;i<cars_speed_group.size();i++)
    // {
    //     for(int j =0;j<cars_speed_group[i].size();j++)
    //     std::cout<<cars_speed_group[i][j].maxspeed<<std::endl;
    // }

    for (auto &groups : cars_speed_group)
    {
        auto comp = [](Car car1, Car car2) {
            // if (car1.is_dir_type_set == car2.is_dir_type_set)
            //     return car1.dj_time < car2.dj_time;
            return car1.is_dir_type_set < car2.is_dir_type_set;
            //return car1.maxspeed > car2.maxspeed;
        };

        std::sort(groups.begin(), groups.end(), comp);
    }

    //  exit(0);
    return 0;
    // for (auto &groups : cars_speed_group)
    // {
    //     auto comp = [](Car car1, Car car2) {
    //         if (car1.is_dir_type_set == car2.is_dir_type_set)
    //         {
    //             //暂不考虑计划出发时间的影响
    //             // if (car1.start_time == car2.start_time)
    //             //     return car1.maxspeed > car2.maxspeed;
    //             return car1.maxspeed > car2.maxspeed;
    //         }
    //         else
    //             return car1.is_dir_type_set > car2.is_dir_type_set;
    //     };

    //     std::sort(groups.begin(), groups.end(), comp);
    // }

    // int count = 0;
    // for (int i = 0; i < 4; i++)
    // {
    //     count += cars_group[i].size();
    // }
    // if (count == Car::Cars.size())
    // {
    //     return 0;
    // }
    // else
    // {
    //     std::cout << "error divide group" << std::endl;
    //     return -1;
    // }
}

int Divide_Group(std::vector<std::vector<Car>> &cars_group)
{
    for (auto car : Car::Cars)
    {
        cars_group[car.dir_type].push_back(car);
        if (car.dir_type < 0 || car.dir_type > 3)
        {
            std::cout << "dir_type error: " << car.dir_type << std::endl;
            exit(0);
        }
    }

    for (auto &groups : cars_group)
    {
        auto comp = [](Car car1, Car car2) {
            return car1.maxspeed > car2.maxspeed;
            // if (car1.is_dir_type_set == car2.is_dir_type_set)
            // {
            //     //暂不考虑计划出发时间的影响
            //     // if (car1.start_time == car2.start_time)
            //     //     return car1.maxspeed > car2.maxspeed;
            //     return car1.maxspeed > car2.maxspeed;
            // }
            // else
            //     return car1.is_dir_type_set > car2.is_dir_type_set;
        };

        std::sort(groups.begin(), groups.end(), comp);
    }

    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        count += cars_group[i].size();
    }
    if (count == Car::Cars.size())
    {
        return 0;
    }
    else
    {
        std::cout << "error divide group" << std::endl;
        return -1;
    }
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

int Car_findpos_by_id(int carid)
{
    if (carid == -1)
        return -1;
    static std::unordered_map<int, int> map; //使用unordered_map存储
    if (map.empty())
    {
        for (int i = 0; i < Car::Cars.size(); i++)
            map[Car::Cars[i].id] = i;
    }
    return map[carid];
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