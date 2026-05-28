#ifndef GRAPHFINDER_DIJKSTRA_H
#define GRAPHFINDER_DIJKSTRA_H

#include "data.h"
#include "commandParse.h"

constexpr int INF = 0x3f3f3f3f;

struct PathResult {
    int node_count = 0;
    int route[N];
    int total_distance = 0;
    int total_time = 0;
    int total_cost = 0;
    bool feasible = true;
};

PathResult shortestPath(int start, int end, int weight_type);

PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
);

PathResult waypointPath(
    int start, int end,
    int waypoints[N],
    int weight_type
);

PathResult toiletAwarePath(
    int start, int end,
    int weight_type,
    bool has_constraint = false,
    int constraint_type = -1,
    int constraint_limit = 0
);

PathResult solve(const RouteRequest& request);

void printPath(const PathResult& path);

#endif // GRAPHFINDER_DIJKSTRA_H
