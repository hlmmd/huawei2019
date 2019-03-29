#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


class Optimize{
public:
    static std::vector<std::vector<std::vector<double>>>dist_time; //cs
    static std::vector<std::vector<double>>scores;
    static std::vector<std::vector<std::vector<std::vector<int>>>>dist_table;
    static std::unordered_map<int,int>speed_map;
    static std::unordered_set<int>speed_set;

    void normalize(std::vector<double> &value);
    bool random_change();
    void sort_ops(std::vector<int> &,std::vector<double> &);
    int update_scores(int time_sche);           //use work time
    double get_score_road(int roadid,int crossid,int carid);
    Optimize(){}
};
