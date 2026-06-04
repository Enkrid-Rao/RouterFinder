#include "include/Dijkstra.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

//标准 Dijkstra O(N²)
PathResult shortestPath(int start, int end, int weight_type)
{
    int dist[N];
    int prev[N];
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
        if (u == end)
        {
            break;
        }

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

    if (dist[end] == INF)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

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

    int td = 0, tt = 0, tc = 0;
    for (int i = 0; i < cnt - 1; i++) {
        int a = temp[i], b = temp[i + 1];
        td += graph[0][a][b];
        tt += graph[1][a][b];
        tc += graph[2][a][b];
    }

    PathResult res;
    res.node_count = cnt;
    for (int i = 0; i < cnt; i++)
    {
        res.route[i] = temp[i];
    }
    res.total_distance = td;
    res.total_time = tt;
    res.total_cost = tc;
    res.feasible = true;
    return res;
}

//约束最短路径, Label-setting + Pareto 剪枝
//optimize_type 是要最小化的维度, constraint_type 是有上限的维度
#define ML 16

PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
)
{
    static int opt_val[N][ML];
    static int cons_val[N][ML];
    static int prev_node[N][ML];
    static int prev_label[N][ML];
    static int label_cnt[N];
    static int min_opt[N];
    static bool settled[N][ML];

    for (int i = 0; i < cityNum; i++) {
        label_cnt[i] = 0;
        min_opt[i] = INF;
        for (int k = 0; k < ML; k++) {
            settled[i][k] = false;
        }
    }

    opt_val[start][0] = 0;
    cons_val[start][0] = 0;
    prev_node[start][0] = -1;
    prev_label[start][0] = -1;
    label_cnt[start] = 1;
    min_opt[start] = 0;

    while (1) {
        int best_u = -1, best_k = -1, best_opt = INF;
        for (int u = 0; u < cityNum; u++) {
            if (min_opt[u] >= best_opt)
            {
                continue;
            }
            for (int k = 0; k < label_cnt[u]; k++) {
                if (!settled[u][k] && opt_val[u][k] < best_opt) {
                    best_opt = opt_val[u][k];
                    best_u = u;
                    best_k = k;
                }
            }
        }

        if (best_u == -1)
        {
            break;
        }

        settled[best_u][best_k] = true;
        min_opt[best_u] = INF;
        for (int k = 0; k < label_cnt[best_u]; k++) {
            if (!settled[best_u][k] && opt_val[best_u][k] < min_opt[best_u]) {
                min_opt[best_u] = opt_val[best_u][k];
            }
        }

        for (int v = 0; v < cityNum; v++) {
            int w_opt = graph[optimize_type][best_u][v];
            int w_cons = graph[constraint_type][best_u][v];
            if (w_opt == INF || w_opt == 0)
            {
                continue;
            }
            if (w_cons == INF || w_cons == 0)
            {
                continue;
            }

            int new_opt = opt_val[best_u][best_k] + w_opt;
            int new_cons = cons_val[best_u][best_k] + w_cons;
            if (new_cons > constraint_limit)
            {
                continue;
            }

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

            if (label_cnt[v] == ML)
            {
                continue;
            }

            int keep = 0;
            int done = 0;
            for (int k = 0; k < label_cnt[v]; k++) {
                if (new_opt <= opt_val[v][k] && new_cons <= cons_val[v][k]) {
                    continue;
                }
                if (!done && new_opt < opt_val[v][k]) {
                    opt_val[v][keep] = new_opt;
                    cons_val[v][keep] = new_cons;
                    prev_node[v][keep] = best_u;
                    prev_label[v][keep] = best_k;
                    settled[v][keep] = false;
                    keep++;
                    done = 1;
                }
                opt_val[v][keep] = opt_val[v][k];
                cons_val[v][keep] = cons_val[v][k];
                prev_node[v][keep] = prev_node[v][k];
                prev_label[v][keep] = prev_label[v][k];
                settled[v][keep] = settled[v][k];
                keep++;
            }
            if (!done)
            {
                opt_val[v][keep] = new_opt;
                cons_val[v][keep] = new_cons;
                prev_node[v][keep] = best_u;
                prev_label[v][keep] = best_k;
                settled[v][keep] = false;
                keep++;
            }
            label_cnt[v] = keep;
            if (new_opt < min_opt[v]) {
                min_opt[v] = new_opt;
            }
        }
    }

    int best_k = -1;
    int best_opt = INF;
    for (int k = 0; k < label_cnt[end]; k++) {
        if (opt_val[end][k] < best_opt) {
            best_opt = opt_val[end][k];
            best_k = k;
        }
    }
    if (best_k == -1)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

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

    PathResult res;
    res.node_count = cnt;
    res.feasible = true;
    for (int i = 0; i < cnt; i++)
    {
        res.route[i] = temp[i];
    }
    res.total_distance = 0;
    res.total_time = 0;
    res.total_cost = 0;
    for (int i = 0; i < cnt - 1; i++) {
        int a = temp[i], b = temp[i + 1];
        res.total_distance += graph[0][a][b];
        res.total_time += graph[1][a][b];
        res.total_cost += graph[2][a][b];
    }
    return res;
}

#undef ML

//途经点路径: start→wp[0]→...→wp[cnt-1]→end, 逐段拼
PathResult waypointPath(
    int start, int end,
    const int waypoints[],
    int waypoint_count,
    int weight_type
) {
    if (waypoint_count == 0)
    {
        return shortestPath(start, end, weight_type);
    }

    PathResult total;
    total.node_count = 0;
    total.total_distance = 0;
    total.total_time = 0;
    total.total_cost = 0;
    total.feasible = true;

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
    total.total_time += seg.total_time;
    total.total_cost += seg.total_cost;

    for (int i = 0; i < waypoint_count - 1; i++) {
        seg = shortestPath(waypoints[i], waypoints[i + 1], weight_type);
        if (!seg.feasible)
        {
            total.feasible = false;
            return total;
        }
        for (int j = 1; j < seg.node_count; j++) {
            total.route[total.node_count++] = seg.route[j];
        }
        total.total_distance += seg.total_distance;
        total.total_time += seg.total_time;
        total.total_cost += seg.total_cost;
    }

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
    total.total_time += seg.total_time;
    total.total_cost += seg.total_cost;

    return total;
}

//厕所优先: 先找最近厕所, 再去终点
PathResult toiletAwarePath(
    int start, int end,
    int weight_type,
    bool has_constraint,
    int constraint_type,
    int constraint_limit
) {
    int toilets[N];
    int tcnt = 0;
    for (int i = 0; i < cityNum; i++) {
        if (wc[i]) {
            toilets[tcnt++] = i;
        }
    }

    if (tcnt == 0)
    {
        if (has_constraint)
        {
            return constrainedShortestPath(start, end, weight_type,
                                           constraint_type, constraint_limit);
        }
        return shortestPath(start, end, weight_type);
    }

    int dist[N], prev[N];
    bool vis[N];
    for (int i = 0; i < cityNum; i++) {
        vis[i] = false;
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

    int nearest = -1;
    for (int i = 0; i < tcnt; i++) {
        int t = toilets[i];
        if (dist[t] != INF && (nearest == -1 || dist[t] < dist[nearest])) {
            nearest = t;
        }
    }
    if (nearest == -1)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

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

    int first_cons = 0;
    if (has_constraint)
    {
        for (int i = 0; i < fcnt - 1; i++) {
            first_cons += graph[constraint_type][first[i]][first[i + 1]];
        }
        if (first_cons > constraint_limit)
        {
            PathResult fail;
            fail.feasible = false;
            return fail;
        }
    }

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

    PathResult total;
    total.node_count = 0;
    total.feasible = true;
    for (int j = 0; j < fcnt; j++) {
        total.route[total.node_count++] = first[j];
    }
    for (int j = 1; j < second.node_count; j++) {
        total.route[total.node_count++] = second.route[j];
    }

    total.total_distance = 0;
    total.total_time = 0;
    total.total_cost = 0;
    for (int i = 0; i < total.node_count - 1; i++) {
        int a = total.route[i], b = total.route[i + 1];
        total.total_distance += graph[0][a][b];
        total.total_time += graph[1][a][b];
        total.total_cost += graph[2][a][b];
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