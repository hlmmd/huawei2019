

#include "common.h"

//静态成员变量需要声明
std::vector<Car> Car::Cars;
std::vector<Car> Car::Answer;    ///cs
std::vector<Car> Car::Answer_bk;    ///cs
// int Car::set_dir_seq()
// {

// }

void Car::init(){
    state = 0;
//	plane_time = 0;
    curRoad = -1, nextCrossId = src;
    wait = false;
    x = 0, y = 0;
    routeIndex = -1;
}

int Car::set_dir_type()
{
    int cross_id = src;
    for (int i = 0; i < road_seq.size(); i++)
    {
        int pos = Cross_findpos_by_id(cross_id);
        for (int j = 0; j < ROADS_OF_CROSS; j++)
        {
            if (Cross::Crosses[pos].dir[j] == road_seq[i])
            {
                //int road_pos = Road_findpos_by_id(road_seq[i]);
                cross_id = Get_next_cross_id(cross_id, road_seq[i]);
                dir_seq.push_back(j);
                break;
            }
        }
    }

    std::set<int> seq_set(dir_seq.begin(), dir_seq.end());

    if (seq_set.size() <= 2)
    {
        if (seq_set.find(0) != seq_set.end() && seq_set.find(3) != seq_set.end())
            dir_type = 3;
        else
        {
            dir_type = *seq_set.begin();
        }
        is_dir_type_set = true;
        return dir_type;
    }

    std::vector<int> count(4, 0);
    for (int i = 0; i < dir_seq.size(); i++)
        count[dir_seq[i]]++;

    std::vector<int> ret = TopKFrequent(dir_seq, 2);

    if (ret[0] == 0 && ret[1] == 1 || ret[0] == 1 && ret[1] == 0)
        dir_type = 0;
    else if (ret[0] == 0 && ret[1] == 3 || ret[0] == 3 && ret[1] == 0)
        dir_type = 3;
    else if (ret[0] == 2 && ret[1] == 1 || ret[0] == 1 && ret[1] == 2)
        dir_type = 1;
    else if (ret[0] == 2 && ret[1] == 3 || ret[0] == 3 && ret[1] == 2)
        dir_type = 2;
    else
    {
        dir_type = rand() % 4;
    }

    is_dir_type_set = false;
    return dir_type;

    // for (int i = 0; i < dir_seq.size(); i++)
    // {
    //     std::cout << dir_seq[i] << " ";
    // }
    // std::cout << std::endl;
}

//当一个方向无法搜索到路径时，更换方向。
int Car::get_next_dir_type()
{
    dir_type = (dir_type + 1) % 4;
    return dir_type;
}

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

    dir_type = -1;
    is_dir_type_set = false;
    started = false;

    dj_time = 0;

//zhao
    state = 0;
	plane_time = 0;
	curRoad = -1, nextCrossId = src;
	wait = false;
	x = 0, y = 0;
	routeIndex = -1;
	changes=0;
//end


    return 0;
}

void Car::Display()
{
  //  std::cout << id << " " << src << " " << dst << " " << maxspeed << " " << start_time << is_dir_type_set << " " << std::endl;
  std::cout <<is_dir_type_set << " "<<maxspeed<<" "<<start_time << std::endl;


    // for (int i = 0; i < road_seq.size(); i++)
    //     std::cout << road_seq[i] << " ";
    // std::cout << std::endl;
    // for (int i = 0; i < dir_seq.size(); i++)
    //     std::cout << dir_seq[i] << " ";
    // std::cout << std::endl;
}

int Car::getpath(std::vector<int> &path, int src_cross, int dst_cross)
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
            Cross::Crosses[s.top()].cars_pass_num++;
            int road_id = Get_Road_by_Two_crossid(Cross::Crosses[old].id, Cross::Crosses[s.top()].id);
            road_seq.push_back(road_id);
        }
    }
    std::set<int> st(road_seq.begin(), road_seq.end());
    road_set = st;
    //   std::cout << std::endl;
    return 0;
}

int Car::CalDijkstraPath_withdir()
{
    std::vector<double> dist(Cross::Crosses.size(), 9999999);
    std::vector<bool> visited(Cross::Crosses.size(), false);
    std::vector<int> path(Cross::Crosses.size(), -1);

    int v0 = Cross_findpos_by_id(src);
    dist[v0] = 0;
    path[v0] = v0;

    //根据dir_type确定搜索的方向
    std::vector<int> road_dir(2, -1);
    if (dir_type == 0)
    {
        road_dir[0] = 0;
        road_dir[1] = 1;
    }
    else if (dir_type == 1)
    {
        road_dir[0] = 1;
        road_dir[1] = 2;
    }
    else if (dir_type == 2)
    {
        road_dir[0] = 2;
        road_dir[1] = 3;
    }
    else if (dir_type == 3)
    {
        road_dir[0] = 3;
        road_dir[1] = 0;
    }
    else
    {
        std::cout << "error dir_type:" << dir_type << std::endl;
        exit(0);
    }

    for (int ii = 0; ii < 2; ii++)
    {

        int i = road_dir[ii];
        //判断起始点能到达哪些路口
        int pos = Cross_findpos_by_id(src);
        if (Cross::Crosses[pos].dir[i] != -1)
        {
            //如果路存在，则根据最大车速、道路限速、道路长度计算出该段距离，作为两个路口间的最短路径。
            int roadpos = Road_findpos_by_id(Cross::Crosses[pos].dir[i]);
            int roadlength = Road::Roads[roadpos].length;
            int speed = std::min(maxspeed, Road::Roads[roadpos].maxspeed); //比较车速和限速，取较小值

            //这里存在一个问题，道路长度能否整除speed?如果不能，则向上取整?
            //路可能是双向的，注意判断起点终点

            //单向路不做处理
            if (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id && Road::Roads[roadpos].is_dup == 0)
                continue;

            int dst_id = (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id) ? Road::Roads[roadpos].src_cross : Road::Roads[roadpos].dst_cross;

            int dist_pos = Cross_findpos_by_id(dst_id);

            dist[dist_pos] = ((double)roadlength / speed);
            //dist[dist_pos] = (roadlength / speed) + ((roadlength % speed) != 0);

            //  std::cout << pos << " " << roadpos << " " << dst_id << " "<<roadlength<<" "<<speed<<" "<<dist[dist_pos]<<std::endl;
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
                road_priority = Cross::Crosses[i].Cal_priority();
            }
            //如果有相同的路径，如何选择?考虑道路的拥挤情况
            else if (visited[i] == false &&  IsAlmostEqual(dist[i], tempmin)  && !IsAlmostEqual(dist[i],9999999))
            {
                int prior = Cross::Crosses[i].Cal_priority();
                if (prior < road_priority)
                {
                    minpos = i;
                    road_priority = prior;
                }
            }
        }

        //std::cout<<"aaa"<<std::endl;
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

 if (minpos == INT_MAX)
            return -1;

        if (Cross::Crosses[minpos].id == dst)
        {
            // std::cout<<"AAA"<<std::endl;
            getpath(path, Cross_findpos_by_id(src), Cross_findpos_by_id(dst));
            dj_time =  dist[minpos];
            return dist[minpos];
        }

       

          //  std::cout << minpos << "AAA" << std::endl;

        //一遍循环后，得到一个最小的dist值，这个值必定对应到一个结点的最小路径
        for (int ii = 0; ii < 2; ii++)
        {

            int i = road_dir[ii];
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
                int dist_pos = Cross_findpos_by_id(dst_id);
                if (visited[dist_pos] == false && dist[dist_pos] > (tempmin + tempdist))
                {
                    //只有比原值更小才更新

                    dist[dist_pos] = tempmin + tempdist;
                    path[dist_pos] = pos;

                     //  std::cout << pos << " " << roadpos << " " << dst_id << " "<<roadlength<<" "<<speed<<" "<<dist[dist_pos]<<std::endl;
                }
            }
        }
        //     exit(0);
        visited[minpos] = true;
    }

    return -1;
}

int Car::CalDijkstraPath()
{
    std::vector<double> dist(Cross::Crosses.size(), 9999999);
    std::vector<bool> visited(Cross::Crosses.size(), false);
    std::vector<int> path(Cross::Crosses.size(), -1);

    int v0 = Cross_findpos_by_id(src);
    dist[v0] = 0;
    path[v0] = v0;

    for (int i = 0; i < ROADS_OF_CROSS; i++)
    {
        //判断起始点能到达哪些路口
        int pos = Cross_findpos_by_id(src);
        if (Cross::Crosses[pos].dir[i] != -1)
        {
            //如果路存在，则根据最大车速、道路限速、道路长度计算出该段距离，作为两个路口间的最短路径。
            int roadpos = Road_findpos_by_id(Cross::Crosses[pos].dir[i]);
            int roadlength = Road::Roads[roadpos].length;
            int speed = std::min(maxspeed, Road::Roads[roadpos].maxspeed); //比较车速和限速，取较小值

            //这里存在一个问题，道路长度能否整除speed?如果不能，则向上取整?
            //路可能是双向的，注意判断起点终点

            //单向路不做处理
            if (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id && Road::Roads[roadpos].is_dup == 0)
                continue;

            int dst_id = (Road::Roads[roadpos].dst_cross == Cross::Crosses[pos].id) ? Road::Roads[roadpos].src_cross : Road::Roads[roadpos].dst_cross;

            int dist_pos = Cross_findpos_by_id(dst_id);

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
                road_priority = Cross::Crosses[i].Cal_priority();
            }
            //如果有相同的路径，如何选择?考虑道路的拥挤情况
            else if (visited[i] == false && IsAlmostEqual(dist[i], tempmin)  && !IsAlmostEqual(dist[i],9999999) )
            {

                int prior = Cross::Crosses[i].Cal_priority();
                if (prior < road_priority)
                {
                    minpos = i;
                    road_priority = prior;
                }
            }
        }
        //std::cout<<"aaa"<<std::endl;
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
            getpath(path, Cross_findpos_by_id(src), Cross_findpos_by_id(dst));
            dj_time =  dist[minpos];
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
                int dist_pos = Cross_findpos_by_id(dst_id);
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

//zhao start
// state 0,1,2,3 park wait finish end
void Car::updateDynamic(int state, int x, int y, int curRoad, int roadSpeed, int nextCrossId) {
	if (this->state != 0 || curRoad != -1)
		this->state = state;
	if (curRoad != -1 && this->state != 0 && this->routeIndex < this->route.size())
		this->routeIndex++;

	this->x = x != -1 ? x : this->x;
	this->y = y != -1 ? y : this->y;
	this->curRoad = curRoad != -1 ? curRoad : this->curRoad;
	if (nextCrossId != -1) {
		this->nextCrossId = nextCrossId;
	}
}


void Car::startInit(int plane_time, std::vector<int>& rou){
	this->plane_time = plane_time;
	this->routeIndex = 0;
    route.assign(rou.begin(), rou.end());
}


int Car::getSpeed(){
	if (curRoad == -1)
		return maxspeed;
    return std::min(maxspeed, RoadDict[curRoad]->getSpeed());
}

int Car::getNextRoad(){//cs
    bool debug=false;
    //return routeIndex < route.size() ?route[routeIndex] : -1;
    //if(routeIndex==route.size()-1){
    if(this->changes>2||routeIndex==route.size()-1){
        return routeIndex < route.size() ?route[routeIndex] : -1;
    }
    std::vector<int> choose;
    Optimize optimizer_temp;
    if(!optimizer_temp.random_change()&&!debug){ //no change
        return routeIndex < route.size() ?route[routeIndex] : -1;
    }
    //change
    int next_cross_id=this->nextCrossId;
    int tempint=-1;
    int road_pos_temp=-1;
    for(int i2=0;i2<4;i2++) {
        if (i2 == 0) {
            road_pos_temp=Road_findpos_by_id(CrossDict[next_cross_id]->down);
            if(Road::Roads[road_pos_temp].src_cross==next_cross_id||(Road::Roads[road_pos_temp].dst_cross==next_cross_id&&Road::Roads[road_pos_temp].is_dup)){
                tempint = CrossDict[next_cross_id]->down;
            }
            else{
                tempint=-1;
            }
        } else if (i2 == 1) {
            road_pos_temp=Road_findpos_by_id(CrossDict[next_cross_id]->up);
            if(Road::Roads[road_pos_temp].src_cross==next_cross_id||(Road::Roads[road_pos_temp].dst_cross==next_cross_id&&Road::Roads[road_pos_temp].is_dup)){
                tempint = CrossDict[next_cross_id]->up;
            }
            else{
                tempint=-1;
            }
        } else if (i2 == 2) {
            road_pos_temp=Road_findpos_by_id(CrossDict[next_cross_id]->left);
            if(Road::Roads[road_pos_temp].src_cross==next_cross_id||(Road::Roads[road_pos_temp].dst_cross==next_cross_id&&Road::Roads[road_pos_temp].is_dup)){
                tempint = CrossDict[next_cross_id]->left;
            }
            else{
                tempint=-1;
            }
        } else if (i2 == 3) {
            road_pos_temp=Road_findpos_by_id(CrossDict[next_cross_id]->right);
            if(Road::Roads[road_pos_temp].src_cross==next_cross_id||(Road::Roads[road_pos_temp].dst_cross==next_cross_id&&Road::Roads[road_pos_temp].is_dup)){
                tempint = CrossDict[next_cross_id]->right;
            }
            else{
                tempint=-1;
            }
        }
        if (tempint != -1) {
            choose.push_back(tempint);
        }
    }
    std::vector<double> value_1(choose.size(),0);  //score
    std::vector<double> value_2(choose.size(),0);  //situation of roads
    std::vector<double> value_3(choose.size(),0);  //dist
    std::vector<double> value_res(choose.size(),0);
    Optimize optimizer;
    for(int i2=0;i2<choose.size();i2++){   //choose[i] road id
        value_1[i2]=Optimize::scores[Road_findpos_by_id(choose[i2])][Cross_findpos_by_id(this->dst)];
    }
    for(int i2=0;i2<choose.size();i2++){
        value_2[i2]=optimizer.get_score_road(choose[i2],next_cross_id,this->id);
    }
    for(int i2=0;i2<choose.size();i2++){
        value_3[i2]=Optimize::dist_time[Optimize::speed_map[this->maxspeed]][Cross_findpos_by_id(Road::Roads[Road_findpos_by_id(choose[i2])].dst_cross)][Cross_findpos_by_id(this->dst)];
    }
    for(int i2=0;i2<choose.size();i2++){
        value_res[i2]=-(para_v1*value_1[i2]+para_v2*value_2[i2]+para_v3*value_3[i2]);
    }
    if(Time>=200){
        int testcs=1;
    }
    optimizer_temp.normalize(value_res);
    optimizer_temp.sort_ops(choose,value_res);
    int choice=0;
    for(;choice<choose.size();choice++){
        if(!routeIndex){
            break;
        }
        if(choose[choice]!=route[routeIndex]) {
            break;
        }
    }
    if(choice==choose.size()){
        return routeIndex < route.size() ?route[routeIndex] : -1;
    }
    int new_cross_id;
    if(next_cross_id==Road::Roads[Road_findpos_by_id(choose[choice])].src_cross){
        new_cross_id=Road::Roads[Road_findpos_by_id(choose[choice])].dst_cross;
    }
    else{
        new_cross_id=Road::Roads[Road_findpos_by_id(choose[choice])].src_cross;
    }
    //route[routeIndex]=choose[choice];
    std::vector<int> new_seq;
    new_seq.assign(route.begin(),route.begin()+routeIndex);
    new_seq.push_back(choose[choice]);
    for(auto t:Optimize::dist_table[Optimize::speed_map[this->maxspeed]][Cross_findpos_by_id(new_cross_id)][Cross_findpos_by_id(this->dst)]){
        new_seq.push_back(t);
    }
    if(new_seq.size()==route.size()){
        route=new_seq;
        this->changes++;
        //std::cout<<"changed"<<std::endl;
    }

    return routeIndex < route.size() ?route[routeIndex] : -1;
    //xiugai route
//	return route[routeIndex];
}

//end