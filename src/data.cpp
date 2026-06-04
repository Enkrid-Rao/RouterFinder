#include"include/data.h"
#include<sstream>
#include<fstream>

// Build graph and name_to_id from raw input
// Input format: city name and toilet status first, then edges:
// CityA,CityB,distance,time,cost

int cityNum = 0;
int graph[3][N][N] = {0};   // initialized to 0
bool wc[N] = {false};
std::unordered_map<std::string, int> name_to_id;
std::unordered_map<int, std::string> id_to_name;

void readWc(){
    std::string filepath = "./wc.csv";
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
        name_to_id[row[0]]=cityNum;
        id_to_name[cityNum]=row[0];
        wc[cityNum]=row[1]=="true";
        cityNum++;
    }
}

// Read edges from file and fill graph and name_to_id
void readEdge(){
    std::string filepath = "./data.csv";
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
        int money=std::stoi(row[4]);
        graph[0][name_to_id[cityA]][name_to_id[cityB]]=distance;
        graph[1][name_to_id[cityA]][name_to_id[cityB]]=time;
        graph[2][name_to_id[cityA]][name_to_id[cityB]]=money;
    }
    file.close();
}
