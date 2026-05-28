#include"include/data.h"
#include<sstream>
#include<fstream>

// 通过原始输入得到 graph 和 name_to_id
// 这里的输入格式可以根据需要进行调整, 目前假设输入格式为:

// 先是每个城市的名称和是否有厕所, 然后是每条边的信息, 包括两个城市的名称, 路程, 时间, 费用, 是否有厕所 

// 城市A 城市B 路程 时间 费用
 
int cityNum = 0;
int graph[4][N][N] = {0};   // 初始化为0
bool wc[N] = {false};
std::unordered_map<std::string, int> name_to_id;
std::unordered_map<int, std::string> id_to_name;

void readWc(){
    std::string filepath = ".\wc.csv";
    std::ifstream file(filepath);

    std::vector<std::vector<std::string>> data;
    std::string line;
    while(std::getline(file,line)){
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while(std::getline(ss,cell,',')){
            row.push_back(cell);
        }
        data.push_back(row);
    }

    for(const auto& row:data){
        name_to_id[row[0]]=++cityNum;
        id_to_name[cityNum]=row[0];
        wc[cityNum]=row[1]=="true";
    }
}

// 从文件中读取数据并填充 graph 和 name_to_id
void readEdge(){
    std::string filepath = ".\data.csv";
    std::ifstream file(filepath);

    std::vector<std::vector<std::string>> data;
    std::string line;

    while(std::getline(file,line)){
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while(std::getline(ss,cell,',')){
            row.push_back(cell);
        }
        data.push_back(row);
    }

    for(const auto& row:data){
        if(row.size()<5) continue;
        std::string cityA=row[0];
        std::string cityB=row[1];
        int distance=std::stoi(row[2]);
        int time=std::stoi(row[3]);
        double money=std::stod(row[4]);
        graph[0][name_to_id[cityA]][name_to_id[cityB]]=distance;
        graph[1][name_to_id[cityA]][name_to_id[cityB]]=time;
        graph[2][name_to_id[cityA]][name_to_id[cityB]]=money;
    }
    file.close();
}