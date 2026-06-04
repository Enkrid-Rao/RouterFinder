#include "include/Dijkstra.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

// ========== 标准 Dijkstra O(N²) ==========
// 以 graph[weight_type][][] 为权重, 求 start→end 最短路径
PathResult shortestPath(int start, int end, int weight_type)
{
    int dist[N];   // 起点到各点的距离
    int prev[N];   // 前驱节点
    bool vis[N];   // 是否已确定最短距离

    //初始化
    for (int i = 0; i < cityNum; i++) {
        vis[i]  = false;
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    //主循环: 每次锁定一个最小 dist 的未访问节点, 更新其邻接边最短距离
    for (int i = 0; i < cityNum; i++) {
        //找当前未访问节点中 dist 最小的
        int u = -1;
        for (int j = 0; j < cityNum; j++) {
            if (!vis[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (u == -1 || dist[u] == INF)   // 不可达
        {
            break;
        }
        vis[u] = true;
        if (u == end)                    // 终点已锁定, 提前结束
        {
            break;
        }

        //更新u的所有邻接边的最短距离
        for (int v = 0; v < cityNum; v++) {
            int w = graph[weight_type][u][v];
            if (vis[v] || w == INF || w == 0)   // 跳过已锁定的和无边的
            {
                continue;
            }
            int nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
            }
        }
    }

    //不可达
    if (dist[end] == INF)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    //从终点沿 prev 走到起点, 再反转
    int temp[N];
    int cnt = 0;
    for (int v = end; v != -1; v = prev[v]) {
        temp[cnt++] = v;
    }
    for (int i = 0; i < cnt / 2; i++) {
        int t = temp[i];
        temp[i] = temp[cnt - 1 - i];
        temp[cnt - 1 - i] = t;
    }

    //沿路径累计 路程/时间/费用
    int total_dist = 0, total_time = 0, total_cost = 0;
    for (int i = 0; i < cnt - 1; i++) {
        int a = temp[i], b = temp[i + 1];
        total_dist += graph[0][a][b];
        total_time += graph[1][a][b];
        total_cost += graph[2][a][b];
    }

    //传入结果
    PathResult res;
    res.node_count = cnt;
    for (int i = 0; i < cnt; i++)
    {
        res.route[i] = temp[i];
    }
    res.total_distance = total_dist;
    res.total_time    = total_time;
    res.total_cost    = total_cost;
    res.feasible      = true;
    return res;
}

// ========== 约束最短路径 (Label-setting + Pareto dominance) ==========
PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
) 
{

}

// ========== 途经点路径 ==========
// start → waypoints[0] → ... → waypoints[count-1] → end
// 每段调用 shortestPath 拼接, 任一段不可达则整体失败
PathResult waypointPath(
    int start, int end,
    const int waypoints[],
    int waypoint_count,
    int weight_type
) {
    //无途经点, 直接走最短路径
    if (waypoint_count == 0)
    {
        return shortestPath(start, end, weight_type);
    }

    //逐段拼接
    PathResult total;
    total.node_count     = 0;
    total.total_distance = 0;
    total.total_time     = 0;
    total.total_cost     = 0;
    total.feasible       = true;

    //第一段: start → wp[0]
    PathResult seg = shortestPath(start, waypoints[0], weight_type);
    if (!seg.feasible)
    {
        total.feasible = false;
        return total;
    }
    for (int j = 0; j < seg.node_count; j++) {
        total.route[total.node_count++] = seg.route[j];
    }
    total.total_distance += seg.total_distance;
    total.total_time    += seg.total_time;
    total.total_cost    += seg.total_cost;

    //中间段: wp[i] → wp[i+1], 跳过起点避免重复
    for (int i = 0; i < waypoint_count - 1; i++) {
        seg = shortestPath(waypoints[i], waypoints[i + 1], weight_type);
        if (!seg.feasible)
        {
            total.feasible = false;
            return total;
        }
        for (int j = 1; j < seg.node_count; j++) {   // j=1 跳过起点
            total.route[total.node_count++] = seg.route[j];
        }
        total.total_distance += seg.total_distance;
        total.total_time    += seg.total_time;
        total.total_cost    += seg.total_cost;
    }

    //最后一段: wp[count-1] → end, 跳过起点避免重复
    seg = shortestPath(waypoints[waypoint_count - 1], end, weight_type);
    if (!seg.feasible)
    {
        total.feasible = false;
        return total;
    }
    for (int j = 1; j < seg.node_count; j++) {
        total.route[total.node_count++] = seg.route[j];
    }
    total.total_distance += seg.total_distance;
    total.total_time    += seg.total_time;
    total.total_cost    += seg.total_cost;

    return total;
}

// ========== 厕所优先路径 ==========
// 先找离起点最近的厕所, 再去终点: start → 最近厕所 → end
// 第二段可带约束, 任一段不可达则整体失败
PathResult toiletAwarePath(
    int start, int end,
    int weight_type,
    bool has_constraint,
    int constraint_type,
    int constraint_limit
) {
    //收集所有有厕所的城市
    int toilets[N];
    int tcnt = 0;
    for (int i = 0; i < cityNum; i++) {
        if (wc[i]) {
            toilets[tcnt++] = i;
        }
    }

    //没有厕所, 直接走
    if (tcnt == 0)
    {
        if (has_constraint)
        {
            return constrainedShortestPath(start, end, weight_type,
                                           constraint_type, constraint_limit);
        }
        return shortestPath(start, end, weight_type);
    }

    //跑一次 Dijkstra 找最近的厕所
    int dist[N], prev[N];
    bool vis[N];
    for (int i = 0; i < cityNum; i++) {
        vis[i]  = false;
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    for (int i = 0; i < cityNum; i++) {
        int u = -1;
        for (int j = 0; j < cityNum; j++) {
            if (!vis[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }
        if (u == -1 || dist[u] == INF)
        {
            break;
        }
        vis[u] = true;

        for (int v = 0; v < cityNum; v++) {
            int w = graph[weight_type][u][v];
            if (vis[v] || w == INF || w == 0)
            {
                continue;
            }
            int nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
            }
        }
    }

    //找最近的可达厕所
    int nearest = -1;
    for (int i = 0; i < tcnt; i++) {
        int t = toilets[i];
        if (dist[t] != INF && (nearest == -1 || dist[t] < dist[nearest])) {
            nearest = t;
        }
    }
    if (nearest == -1)   // 所有厕所都不可达
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    //重建 start→nearest 路径
    int first[N];
    int fcnt = 0;
    for (int v = nearest; v != -1; v = prev[v]) {
        first[fcnt++] = v;
    }
    for (int i = 0; i < fcnt / 2; i++) {
        int t = first[i];
        first[i] = first[fcnt - 1 - i];
        first[fcnt - 1 - i] = t;
    }

    //第二段: nearest → end
    PathResult second;
    if (has_constraint)
    {
        second = constrainedShortestPath(nearest, end, weight_type,
                                         constraint_type, constraint_limit);
    }
    else
    {
        second = shortestPath(nearest, end, weight_type);
    }
    if (!second.feasible)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    //拼接: 第一段完整 + 第二段跳过起点
    PathResult total;
    total.node_count = 0;
    total.feasible   = true;
    for (int j = 0; j < fcnt; j++) {
        total.route[total.node_count++] = first[j];
    }
    for (int j = 1; j < second.node_count; j++) {
        total.route[total.node_count++] = second.route[j];
    }

    //从最终路径累加三种权重
    total.total_distance = 0;
    total.total_time     = 0;
    total.total_cost     = 0;
    for (int i = 0; i < total.node_count - 1; i++) {
        int a = total.route[i], b = total.route[i + 1];
        total.total_distance += graph[0][a][b];
        total.total_time    += graph[1][a][b];
        total.total_cost    += graph[2][a][b];
    }

    return total;
}

// ========== 统一调度入口 ==========
// 这个和下面那个是交互,我来写
PathResult solve(const RouteRequest& request) {

}

// ========== 输出 ==========
void printPath(const PathResult& path) {

}
