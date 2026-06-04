#include "include/commandParse.h"
#include "include/data.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Parse command line arguments, return true on success, false on failure
bool parseCommandLine(int argc, char* argv[], RouteRequest& req)
{
    req.start_id = -1;
    req.end_id   = -1;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-')   // not --xxx format, skip
        {
            continue;
        }

        const char* flag = argv[i] + 2;   // skip leading --

        // --srt start city name
        if (strcmp(flag, "srt") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --srt requires a city name\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("Error: city '%s' not found\n", name);
                return false;
            }
            req.start_id = it->second;
        }
        // --dst destination city name
        else if (strcmp(flag, "dst") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --dst requires a city name\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("Error: city '%s' not found\n", name);
                return false;
            }
            req.end_id = it->second;
        }
        // Optimization target: --t time --d distance --c cost
        else if (strcmp(flag, "t") == 0)
        {
            req.optimize = WeightDim::TIME;
        }
        else if (strcmp(flag, "d") == 0)
        {
            req.optimize = WeightDim::DISTANCE;
        }
        else if (strcmp(flag, "c") == 0)
        {
            req.optimize = WeightDim::COST;
        }
        // Constraints: --Td <limit>  min distance under time constraint
        else if (strcmp(flag, "Td") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Td requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::TIME;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::DISTANCE;
        }
        else if (strcmp(flag, "Tc") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Tc requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::TIME;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::COST;
        }
        else if (strcmp(flag, "Dt") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Dt requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::DISTANCE;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::TIME;
        }
        else if (strcmp(flag, "Dc") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Dc requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::DISTANCE;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::COST;
        }
        else if (strcmp(flag, "Ct") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Ct requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::COST;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::TIME;
        }
        else if (strcmp(flag, "Cd") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --Cd requires a limit value\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::COST;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::DISTANCE;
        }
        // --wp waypoint city name
        else if (strcmp(flag, "wp") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("Error: --wp requires a city name\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("Error: city '%s' not found\n", name);
                return false;
            }
            req.waypoints.push_back(it->second);
        }
        // --wc need toilet
        else if (strcmp(flag, "wc") == 0)
        {
            req.need_toilet = true;
        }
        else
        {
            printf("Warning: unknown option '%s'\n", flag);
        }
    }

    if (req.start_id == -1)
    {
        printf("Error: missing --srt start city\n");
        return false;
    }
    if (req.end_id == -1)
    {
        printf("Error: missing --dst destination city\n");
        return false;
    }

    return true;
}
