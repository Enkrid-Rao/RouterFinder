#ifndef GRAPHFINDER_COMMANDPARSE_H
#define GRAPHFINDER_COMMANDPARSE_H

// #include <optional>
#include <string>
#include <vector>

enum class WeightDim : int { DISTANCE = 0, TIME = 1, COST = 2 };

struct RouteRequest {
    int start_id;
    int end_id;

    WeightDim optimize = WeightDim::TIME;

    bool has_constraint = false;
    WeightDim constraint_dim = WeightDim::TIME;
    int constraint_limit = 0;

    std::vector<int> waypoints;

    bool need_toilet = false;
};

RouteRequest parseCommandLine();

#endif // GRAPHFINDER_COMMANDPARSE_H
