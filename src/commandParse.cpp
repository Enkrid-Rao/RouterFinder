#include "include/commandParse.h"
#include "include/data.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// 解析命令行参数, 成功 true, 失败 false
bool parseCommandLine(int argc, char* argv[], RouteRequest& req)
{
    req.start_id = -1;
    req.end_id   = -1;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' || argv[i][1] != '-')   // 不是 --xxx 格式, 跳过
        {
            continue;
        }

        const char* flag = argv[i] + 2;   // 跳过开头的 --

        // --srt 起点城市名
        if (strcmp(flag, "srt") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("错误: --srt 需要城市名\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("错误: 城市 '%s' 不存在\n", name);
                return false;
            }
            req.start_id = it->second;
        }
        // --dst 终点城市名
        else if (strcmp(flag, "dst") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("错误: --dst 需要城市名\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("错误: 城市 '%s' 不存在\n", name);
                return false;
            }
            req.end_id = it->second;
        }
        // 优化目标: --t 时间 --d 路程 --c 费用
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
        // 约束类: --Td <limit>  时间约束下路程最短
        else if (strcmp(flag, "Td") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("错误: --Td 需要限制值\n");
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
                printf("错误: --Tc 需要限制值\n");
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
                printf("错误: --Dt 需要限制值\n");
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
                printf("错误: --Dc 需要限制值\n");
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
                printf("错误: --Ct 需要限制值\n");
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
                printf("错误: --Cd 需要限制值\n");
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::COST;
            req.constraint_limit = atoi(argv[++i]);
            req.optimize         = WeightDim::DISTANCE;
        }
        // --wp 途经城市名
        else if (strcmp(flag, "wp") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("错误: --wp 需要城市名\n");
                return false;
            }
            const char* name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end())
            {
                printf("错误: 城市 '%s' 不存在\n", name);
                return false;
            }
            req.waypoints.push_back(it->second);
        }
        // --wc 需要厕所
        else if (strcmp(flag, "wc") == 0)
        {
            req.need_toilet = true;
        }
        else
        {
            printf("警告: 未知选项 '%s'\n", flag);
        }
    }

    if (req.start_id == -1)
    {
        printf("错误: 缺少 --srt 起点\n");
        return false;
    }
    if (req.end_id == -1)
    {
        printf("错误: 缺少 --dst 终点\n");
        return false;
    }

    return true;
}
