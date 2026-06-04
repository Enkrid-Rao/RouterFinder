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

// ========== 约束最短路径 ==========
// 在 constraint_type 维度 ≤ constraint_limit 的条件下, 最小化 optimize_type
// 如 --Td: optimize=distance, constraint=time, limit=规定时间
// 使用 Label-setting + Pareto 支配剪枝
#define MAX_LABELS 16

PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
)
{
    //大数组放 static 区, 避免爆栈 (N=1e4, MAX_LABELS=16 → ~2.5MB)
    static int opt_val[N][MAX_LABELS];     // 优化维度的当前值
    static int cons_val[N][MAX_LABELS];    // 约束维度的消耗值
    static int prev_node[N][MAX_LABELS];   // 前驱节点
    static int prev_label[N][MAX_LABELS];  // 前驱节点的哪一个 label
    static int label_cnt[N];               // 每个节点当前 label 数量
    static int min_opt[N];                 // 每个节点未处理 label 的最小 opt_val
    static bool settled[N][MAX_LABELS];    // label 是否已锁定

    //初始化
    for (int i = 0; i < cityNum; i++) {
        label_cnt[i] = 0;
        min_opt[i]   = INF;
        for (int k = 0; k < MAX_LABELS; k++) {
            settled[i][k] = false;
        }
    }

    opt_val[start][0]   = 0;
    cons_val[start][0]  = 0;
    prev_node[start][0] = -1;
    prev_label[start][0] = -1;
    label_cnt[start]     = 1;
    min_opt[start]       = 0;

    //主循环: 每次锁定 opt_val 最小的未处理 label
    while (1) {
        //找全局 opt_val 最小的未锁定 label
        int best_u = -1, best_k = -1, best_opt = INF;
        for (int u = 0; u < cityNum; u++) {
            if (min_opt[u] >= best_opt)   // 该节点最小的都比当前最优大, 跳过
            {
                continue;
            }
            for (int k = 0; k < label_cnt[u]; k++) {
                if (!settled[u][k] && opt_val[u][k] < best_opt) {
                    best_opt = opt_val[u][k];
                    best_u   = u;
                    best_k   = k;
                }
            }
        }

        if (best_u == -1)   // 没有更多 label 可处理
        {
            break;
        }

        //锁定该 label
        settled[best_u][best_k] = true;
        //更新该节点的 min_opt
        min_opt[best_u] = INF;
        for (int k = 0; k < label_cnt[best_u]; k++) {
            if (!settled[best_u][k] && opt_val[best_u][k] < min_opt[best_u]) {
                min_opt[best_u] = opt_val[best_u][k];
            }
        }

        //扩展 best_u 的所有邻接边
        for (int v = 0; v < cityNum; v++) {
            int w_opt  = graph[optimize_type][best_u][v];
            int w_cons = graph[constraint_type][best_u][v];
            if (w_opt == INF || w_opt == 0)    // 无边
            {
                continue;
            }
            if (w_cons == INF || w_cons == 0)
            {
                continue;
            }

            int new_opt  = opt_val[best_u][best_k]  + w_opt;
            int new_cons = cons_val[best_u][best_k] + w_cons;
            if (new_cons > constraint_limit)       // 超出约束限制
            {
                continue;
            }

            //检查新 label 是否被已有 label 支配
            int dominated = 0;
            for (int k = 0; k < label_cnt[v]; k++) {
                if (opt_val[v][k] <= new_opt && cons_val[v][k] <= new_cons) {
                    dominated = 1;
                    break;
                }
            }
            if (dominated)
            {
                continue;
            }

            //label 已满且无法淘汰旧 label, 丢弃新 label 防止越界
            if (label_cnt[v] == MAX_LABELS)
            {
                continue;
            }

            //移除被新 label 支配的旧 label, 同时写入新 label
            int keep = 0;
            int done = 0;
            for (int k = 0; k < label_cnt[v]; k++) {
                if (new_opt <= opt_val[v][k] && new_cons <= cons_val[v][k]) {
                    continue;   // 新支配旧, 丢弃旧 label
                }
                if (!done && new_opt < opt_val[v][k]) {
                    //在此处插入新 label, 保持按 opt_val 升序
                    opt_val[v][keep]    = new_opt;
                    cons_val[v][keep]   = new_cons;
                    prev_node[v][keep]  = best_u;
                    prev_label[v][keep] = best_k;
                    settled[v][keep]    = false;
                    keep++;
                    done = 1;
                }
                opt_val[v][keep]    = opt_val[v][k];
                cons_val[v][keep]   = cons_val[v][k];
                prev_node[v][keep]  = prev_node[v][k];
                prev_label[v][keep] = prev_label[v][k];
                settled[v][keep]    = settled[v][k];
                keep++;
            }
            //如果新 label 的 opt 最大, 追加到末尾
            if (!done)
            {
                opt_val[v][keep]    = new_opt;
                cons_val[v][keep]   = new_cons;
                prev_node[v][keep]  = best_u;
                prev_label[v][keep] = best_k;
                settled[v][keep]    = false;
                keep++;
            }
            label_cnt[v] = keep;
            //更新 min_opt[v]
            if (new_opt < min_opt[v]) {
                min_opt[v] = new_opt;
            }
        }
    }

    //在终点找 opt_val 最小的 label
    int best_k = -1;
    int best_opt = INF;
    for (int k = 0; k < label_cnt[end]; k++) {
        if (opt_val[end][k] < best_opt) {
            best_opt = opt_val[end][k];
            best_k   = k;
        }
    }
    if (best_k == -1)   // 没有满足约束的路径
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    //回溯路径: 从终点沿 prev_node/prev_label 走到起点
    int temp[N];
    int cnt = 0;
    int cur_u = end, cur_k = best_k;
    while (cur_u != -1) {
        temp[cnt++] = cur_u;
        int nu = prev_node[cur_u][cur_k];
        int nk = prev_label[cur_u][cur_k];
        cur_u = nu;
        cur_k = nk;
    }
    for (int i = 0; i < cnt / 2; i++) {
        int t = temp[i];
        temp[i] = temp[cnt - 1 - i];
        temp[cnt - 1 - i] = t;
    }

    //沿路径累计路程/时间/费用
    PathResult res;
    res.node_count = cnt;
    res.feasible   = true;
    for (int i = 0; i < cnt; i++)
    {
        res.route[i] = temp[i];
    }
    res.total_distance = 0;
    res.total_time     = 0;
    res.total_cost     = 0;
    for (int i = 0; i < cnt - 1; i++) {
        int a = temp[i], b = temp[i + 1];
        res.total_distance += graph[0][a][b];
        res.total_time    += graph[1][a][b];
        res.total_cost    += graph[2][a][b];
    }
    return res;
}

#undef MAX_LABELS

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

    //计算第一段消耗的约束维度的量
    int first_cons = 0;
    if (has_constraint)
    {
        for (int i = 0; i < fcnt - 1; i++) {
            first_cons += graph[constraint_type][first[i]][first[i + 1]];
        }
        if (first_cons > constraint_limit)   // 第一段已超限
        {
            PathResult fail;
            fail.feasible = false;
            return fail;
        }
    }

    //第二段: nearest → end, 约束扣除第一段消耗
    PathResult second;
    if (has_constraint)
    {
        second = constrainedShortestPath(nearest, end, weight_type,
                                         constraint_type,
                                         constraint_limit - first_cons);
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
