#include <fstream>
#include <iostream>
#include <string>


#ifndef CROSS
#define CROSS

#define ROADS_OF_CROSS 4
class Road;

class Cross{
public:
    int id;
    std::vector<int> dir;
    int up;
    int right;
    int down;
    int left;
    std::vector<int> roadIds;
    unordered_map<pair<int,int>, int> directionMap;
    std::vector<int> providerDirection, receiverDirection, validRoadDirecction;
    std::vector<int> provider, receiver, validRoad;
    int parkCarNum, finishnum;
    bool done, update;

    Cross(int i,int u,int r,int d,int l){
        id = i;
        up = u;
        right = r;
        down=d;
        left =l;
        roadIds.push_back(up);
        roadIds.push_back(right);
        roadIds.push_back(down);
        roadIds.push_back(left);
        directionMap[<up, right>] = 1;
        directionMap[<up, down>] = 2;
        directionMap[<up, left>] = -1;

        directionMap[<right, down>] = 1;
        directionMap[<right, left>] = 2;
        directionMap[<right, up>] = -1;

        directionMap[<down, left>] = 1;
        directionMap[<down, up>] = 2;
        directionMap[<down, right>] = -1;                

        directionMap[<left, up>] = 1;
        directionMap[<left, right>] = 2;
        directionMap[<left, down>] = -1;
        unordered_map<int,int> tmp_provider;

        for(int i=0; i<roadIds.size(); i++){
            if(roadIds[i] == -1)
                continue;
            Road road = RoadDict[roadIds[i]];
            if(road.isDuplex() || road.to == this.id){
                provider.push_back(roadIds[i]);
                tmp_provider[roadIds[i]] = i;
            }
            if(road.isDuplex() || road.from == this.id){
                receiverDirection.push_back(i);
                receiver.push_back(roadIds[i]);
            }
            validRoadDirecction.push(i);
            validRoad.push_back(roadIds[i]);
        }

        sort(provider.begin(), provider.end());
        for(int t: provider){
            providerDirection.push_back(tmp_provider[t]);
        }

        parkCarNum = 0;
        finishCarNum = 0;
        done = false;
        update = false;
    }
    Cross()
    {

    }

    int ReadCross(const std::string );
    void Display();
    int RemoveSingleRoad(std::vector<Road> &roads);
};

#endif