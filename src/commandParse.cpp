#include "include/commandParse.h"
#include "include/data.h"
#include <iostream>
#include <string>

// 解析命令行参数, 成功 true, 失败 false
bool parseCommandLine(int argc, char* argv[], RouteRequest& req)
{
    req.start_id = -1;
    req.end_id   = -1;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        // 不是 --xxx 格式, 跳过
        if (arg.size() < 3 || arg[0] != '-' || arg[1] != '-')
            continue;

        std::string flag = arg.substr(2);   // 跳过开头的 --

        // --srt 起点城市名
        if (flag == "srt") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --srt 需要城市名" << std::endl;
                return false;
            }
            std::string name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end()) {
                std::cerr << "错误: 城市 '" << name << "' 不存在" << std::endl;
                return false;
            }
            req.start_id = it->second;
        }
        // --dst 终点城市名
        else if (flag == "dst") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --dst 需要城市名" << std::endl;
                return false;
            }
            std::string name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end()) {
                std::cerr << "错误: 城市 '" << name << "' 不存在" << std::endl;
                return false;
            }
            req.end_id = it->second;
        }
        // 优化目标: --t 时间 --d 路程 --c 费用
        else if (flag == "t") {
            req.optimize = WeightDim::TIME;
        }
        else if (flag == "d") {
            req.optimize = WeightDim::DISTANCE;
        }
        else if (flag == "c") {
            req.optimize = WeightDim::COST;
        }
        // 约束类: --Td <limit>  时间约束下路程最短
        else if (flag == "Td") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Td 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::TIME;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::DISTANCE;
        }
        else if (flag == "Tc") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Tc 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::TIME;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::COST;
        }
        else if (flag == "Dt") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Dt 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::DISTANCE;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::TIME;
        }
        else if (flag == "Dc") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Dc 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::DISTANCE;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::COST;
        }
        else if (flag == "Ct") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Ct 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::COST;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::TIME;
        }
        else if (flag == "Cd") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --Cd 需要限制值" << std::endl;
                return false;
            }
            req.has_constraint   = true;
            req.constraint_dim   = WeightDim::COST;
            req.constraint_limit = std::stoi(argv[++i]);
            req.optimize         = WeightDim::DISTANCE;
        }
        // --wp 途经城市名
        else if (flag == "wp") {
            if (i + 1 >= argc) {
                std::cerr << "错误: --wp 需要城市名" << std::endl;
                return false;
            }
            std::string name = argv[++i];
            auto it = name_to_id.find(name);
            if (it == name_to_id.end()) {
                std::cerr << "错误: 城市 '" << name << "' 不存在" << std::endl;
                return false;
            }
            req.waypoints.push_back(it->second);
        }
        // --wc 需要厕所
        else if (flag == "wc") {
            req.need_toilet = true;
        }
        else {
            std::cerr << "警告: 未知选项 '" << flag << "'" << std::endl;
        }
    }

    if (req.start_id == -1) {
        std::cerr << "错误: 缺少 --srt 起点" << std::endl;
        return false;
    }
    if (req.end_id == -1) {
        std::cerr << "错误: 缺少 --dst 终点" << std::endl;
        return false;
    }

    return true;
}
