#include "common.h"

int Get_Road_by_Two_crossid(std::vector<Road> &roads, std::vector<Cross> &crosses, int cross_src_id, int cross_dst_id)
{
    int pos = Cross_findpos_by_id(crosses, cross_src_id);

    for(int i = 0;i<ROADS_OF_CROSS;i++)
    {
        int road_id = crosses[pos].dir[i];
        if(road_id!=-1)
        {
            int road_pos = Road_findpos_by_id(roads,road_id);
            if(roads[road_pos].dst_cross==cross_dst_id || roads[road_pos].is_dup==1 && roads[road_pos].src_cross==cross_dst_id )
                return road_id;
        }
    }
    
    std::cout<<std::endl<<cross_src_id<<" "<< cross_dst_id<<"Get_Road_by_Two_crossid error"<<std::endl;
    return 0;
}

int Road_findpos_by_id(std::vector<Road> &roads, int roadid)
{
    if(roadid==-1)
        return -1;
    static std::unordered_map<int, int> map; //使用unordered_map存储
    if (map.empty())
    {
        for (int i = 0; i < roads.size(); i++)
            map[roads[i].id] = i;
    }
    return map[roadid];
}

int Cross_findpos_by_id(std::vector<Cross> &crosses, int crossid)
{
    if(crossid==-1)
        return -1;
    static std::unordered_map<int, int> map; //使用unordered_map存储
    if (map.empty())
    {
        for (int i = 0; i < crosses.size(); i++)
            map[crosses[i].id] = i;
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
    if(!out)
        return -1;

    for(auto car:cars)
    {
        out<<'(';
        out<<car.id<<',';
        out<<car.start_time;
        for(auto road_id:car.road_seq)
            out<<','<<road_id;
        // for(int i = 0;i<car.road_seq.size()-1;i++)
        //     out<<car.road_seq[i]<<',';
        // out<<car.road_seq[car.road_seq.size()-1];
        out<<')'<<std::endl;
    }

    return 0;
}