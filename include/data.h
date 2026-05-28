#ifndef GRAPHFINDER_DATA_H
#define GRAPHFINDER_DATA_H

#include <string>
#include <unordered_map>

constexpr int N=1e4;

int graph[4][N][N]; // graph[0]表示路程, graph[1]表示时间, graph[2]表示费用, graph[3]表示综合评分
bool wc[N][N]; // 表示是否有厕所

std::unordered_map<std::string, int> name_to_id; // 记录每个城市的 id
std::unordered_map<int, std::string> id_to_name; // 记录每个 id 对应的城市名称

#endif //GRAPHFINDER_DATA_H