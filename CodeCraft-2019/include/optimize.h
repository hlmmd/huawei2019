#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Optimize
{
  public:
    static std::vector<std::vector<int>> dist; //cs
    static std::vector<std::vector<int>> scores;
    static std::vector<std::vector<std::vector<int>>> dist_table;

    void normalize(std::vector<double> &value);
    bool random_change();
    void sort_ops(std::vector<int> &, std::vector<double> &);
    Optimize() {}
};
