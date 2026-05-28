#ifndef GRAPHFINDER_DIJKSTRA_H
#define GRAPHFINDER_DIJKSTRA_H

#include "data.h"

// 结合data.h中的数据结构, 定义 Dijkstra 结构体来存储输入和输出数据
struct Dijkstra{

    // 输入数据
    int start; // 起点
    int end; // 终点
    int weight_type; // 权重类型: 0-路程, 1-时间, 2-费用, 3-综合评分

    // 输出数据
    int nodes; // 节点数量
    int route[N]; // 存储路径
    int distance; // 总路程
    int time; // 总时间
    int cost; // 总费用


};



#endif //GRAPHFINDER_DIJKSTRA_H