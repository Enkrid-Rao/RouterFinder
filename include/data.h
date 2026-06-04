#ifndef GRAPHFINDER_DATA_H
#define GRAPHFINDER_DATA_H

#include <string>
#include <unordered_map>
#include<vector>

constexpr int N=1e4;

extern int cityNum;
extern int graph[3][N][N]; // graph[0]=distance, graph[1]=time, graph[2]=cost
extern bool wc[N]; // whether the city has a toilet

extern std::unordered_map<std::string, int> name_to_id; // city name to id mapping
extern std::unordered_map<int, std::string> id_to_name; // id to city name mapping

void readWc();
void readEdge();

#endif //GRAPHFINDER_DATA_H
