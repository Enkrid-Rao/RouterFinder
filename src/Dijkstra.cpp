#include "include/Dijkstra.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

// ========== 标准 Dijkstra (O(N²)) ==========
PathResult shortestPath(int start, int end, int weight_type) {

}

// ========== 约束最短路径 (Label-setting + Pareto dominance) ==========
PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
) {

}

// ========== 途经点路径 ==========
PathResult waypointPath(
    int start, int end,
    const std::vector<int>& waypoints,
    int weight_type
) {

}

// ========== 厕所优先路径 ==========
PathResult toiletAwarePath(
    int start, int end,
    int weight_type,
    bool has_constraint,
    int constraint_type,
    int constraint_limit
) {

}

// ========== 统一调度入口 ==========
// 这个和下面那个是交互,我来写
PathResult solve(const RouteRequest& request) {

}

// ========== 输出 ==========
void printPath(const PathResult& path) {

}
