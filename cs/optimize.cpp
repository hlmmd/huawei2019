#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "common.h"

std::vector<std::vector<int>>Optimize::dist;
std::vector<std::vector<int>>Optimize::scores;
std::vector<std::vector<std::vector<int>>>Optimize::dist_table;
//cs funcs
/*int Reset_Group(std::vector<std::vector<Car>> &cars_group){
    for(int i=0;i<cars_group.size();i++){
        for(int j=0;j<cars_group[i].size();j++){
            cars_group[i][j].started= false;
            cars_group[i][j].start_time=Car::Cars[Car_findpos_by_id(cars_group[i][j].id)].start_time_org;
            Car::Cars[Car_findpos_by_id(cars_group[i][j].id)].start_time=Car::Cars[Car_findpos_by_id(cars_group[i][j].id)].start_time_org;
        }
    }
    return 0;
}*/

void Optimize::normalize(std::vector<double> &value){
    double ymax=1;
    double ymin=0;
    double dMaxValue = *max_element(value.begin(),value.end());
    double dMinValue = *min_element(value.begin(),value.end());
    for (int f = 0; f < value.size(); ++f) {
        value[f] = (ymax-ymin)*(value[f]-dMinValue)/(dMaxValue-dMinValue+1e-8)+ymin;
    }
    return;
}

bool Optimize::random_change(){
    srand(time(0));
    int rands=int(1.0/(1.0-para_choose));
    int k=rand()%(rands);
    if(!k){
        return true;
    }
    else{
        return false;
    }
}


void Optimize::sort_ops(std::vector<int> &choose,std::vector<double> &value){
    //std::vector<double>softmax(value.size(),0);
    double sumv=0;
    double maxv=0;
    for(int i=0;i<value.size();i++){
        if(value[i]>maxv){
            maxv=value[i];
        }
    }
    for(int i=0;i<value.size();i++){
        sumv+=value[i];
        value[i]=exp(value[i]-maxv);
    }
    for(int i=0;i<value.size();i++){
        value[i]/=sumv;
    }
    for(int i=0;i<value.size();i++){
        for(int j=1;j<value.size()-i;j++){
            if(value[j-1]<value[j]){
                std::swap(value[j-1],value[j]);
                std::swap(choose[j-1],choose[j]);
            }
        }
    }
}
//cs funcs