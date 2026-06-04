#include "include/Dijkstra.h"
#include <cstdio>
#include <cstring>
#include <vector>

// ========== Standard Dijkstra O(N^2) ==========
// Using graph[weight_type][][] as weight, find shortest path from start to end
PathResult shortestPath(int start, int end, int weight_type)
{
    static int dist[N];   // distance from start to each node
    static int prev[N];   // predecessor node
    static bool vis[N];   // whether shortest distance is determined

    // Initialize
    for (int i = 0; i < cityNum; i++) {
        vis[i]  = false;
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    // Main loop: lock the unvisited node with minimum dist, update its neighbors
    for (int i = 0; i < cityNum; i++) {
        // Find unvisited node with minimum dist
        int u = -1;
        for (int j = 0; j < cityNum; j++) {
            if (!vis[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (u == -1 || dist[u] == INF)   // unreachable
        {
            break;
        }
        vis[u] = true;
        if (u == end)                    // destination locked, early exit
        {
            break;
        }

        // Update shortest distance for all neighbors of u
        for (int v = 0; v < cityNum; v++) {
            int w = graph[weight_type][u][v];
            if (vis[v] || w == INF || w == 0)   // skip settled nodes and non-edges
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

    // Unreachable
    if (dist[end] == INF)
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    // Walk from end to start via prev, then reverse
    static int temp[N];
    int cnt = 0;
    for (int v = end; v != -1; v = prev[v]) {
        temp[cnt++] = v;
    }
    for (int i = 0; i < cnt / 2; i++) {
        int t = temp[i];
        temp[i] = temp[cnt - 1 - i];
        temp[cnt - 1 - i] = t;
    }

    // Accumulate distance/time/cost along the path
    int total_dist = 0, total_time = 0, total_cost = 0;
    for (int i = 0; i < cnt - 1; i++) {
        int a = temp[i], b = temp[i + 1];
        total_dist += graph[0][a][b];
        total_time += graph[1][a][b];
        total_cost += graph[2][a][b];
    }

    // Fill result
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

// ========== Constrained Shortest Path ==========
// Under constraint_dim <= constraint_limit, minimize optimize_type
// e.g. --Td: optimize=distance, constraint=time, limit=given_time
// Uses Label-setting + Pareto dominance pruning
#define MAX_LABELS 16

PathResult constrainedShortestPath(
    int start, int end,
    int optimize_type,
    int constraint_type,
    int constraint_limit
)
{
    // Large arrays in static storage to avoid stack overflow (N=1e4, MAX_LABELS=16 -> ~2.5MB)
    static int opt_val[N][MAX_LABELS];     // current value of optimize dimension
    static int cons_val[N][MAX_LABELS];    // consumption of constraint dimension
    static int prev_node[N][MAX_LABELS];   // predecessor node
    static int prev_label[N][MAX_LABELS];  // which label of predecessor
    static int label_cnt[N];               // current label count per node
    static int min_opt[N];                 // min opt_val among unprocessed labels per node
    static bool settled[N][MAX_LABELS];    // whether label is settled

    // Initialize
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

    // Main loop: lock the unprocessed label with minimum opt_val
    while (1) {
        // Find globally unprocessed label with minimum opt_val
        int best_u = -1, best_k = -1, best_opt = INF;
        for (int u = 0; u < cityNum; u++) {
            if (min_opt[u] >= best_opt)   // smallest at this node is already >= current best, skip
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

        if (best_u == -1)   // no more labels to process
        {
            break;
        }

        // Lock this label
        settled[best_u][best_k] = true;
        // Update min_opt for this node
        min_opt[best_u] = INF;
        for (int k = 0; k < label_cnt[best_u]; k++) {
            if (!settled[best_u][k] && opt_val[best_u][k] < min_opt[best_u]) {
                min_opt[best_u] = opt_val[best_u][k];
            }
        }

        // Extend all outgoing edges from best_u
        for (int v = 0; v < cityNum; v++) {
            int w_opt  = graph[optimize_type][best_u][v];
            int w_cons = graph[constraint_type][best_u][v];
            if (w_opt == INF || w_opt == 0)    // no edge
            {
                continue;
            }
            if (w_cons == INF || w_cons == 0)
            {
                continue;
            }

            int new_opt  = opt_val[best_u][best_k]  + w_opt;
            int new_cons = cons_val[best_u][best_k] + w_cons;
            if (new_cons > constraint_limit)       // exceeds constraint limit
            {
                continue;
            }

            // Check if new label is dominated by existing labels
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

            // Labels full and cannot evict old ones, discard to prevent overflow
            if (label_cnt[v] == MAX_LABELS)
            {
                continue;
            }

            // Remove old labels dominated by new label, insert new label
            int keep = 0;
            int done = 0;
            for (int k = 0; k < label_cnt[v]; k++) {
                if (new_opt <= opt_val[v][k] && new_cons <= cons_val[v][k]) {
                    continue;   // new dominates old, discard old
                }
                if (!done && new_opt < opt_val[v][k]) {
                    // Insert new label here, maintain ascending opt_val order
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
            // If new label has largest opt, append to end
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
            // Update min_opt[v]
            if (new_opt < min_opt[v]) {
                min_opt[v] = new_opt;
            }
        }
    }

    // Find label with minimum opt_val at destination
    int best_k = -1;
    int best_opt = INF;
    for (int k = 0; k < label_cnt[end]; k++) {
        if (opt_val[end][k] < best_opt) {
            best_opt = opt_val[end][k];
            best_k   = k;
        }
    }
    if (best_k == -1)   // no path satisfies constraints
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    // Backtrack: walk from destination to start via prev_node/prev_label
    static int temp[N];
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

    // Accumulate distance/time/cost along the path
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

// ========== Waypoint Path ==========
// start -> waypoints[0] -> ... -> waypoints[count-1] -> end
// Each segment uses shortestPath; overall fails if any segment fails
PathResult waypointPath(
    int start, int end,
    const int waypoints[],
    int waypoint_count,
    int weight_type
) {
    // No waypoints, directly find shortest path
    if (waypoint_count == 0)
    {
        return shortestPath(start, end, weight_type);
    }

    // Concatenate segment by segment
    PathResult total;
    total.node_count     = 0;
    total.total_distance = 0;
    total.total_time     = 0;
    total.total_cost     = 0;
    total.feasible       = true;

    // First leg: start -> wp[0]
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

    // Middle legs: wp[i] -> wp[i+1], skip start to avoid duplicates
    for (int i = 0; i < waypoint_count - 1; i++) {
        seg = shortestPath(waypoints[i], waypoints[i + 1], weight_type);
        if (!seg.feasible)
        {
            total.feasible = false;
            return total;
        }
        for (int j = 1; j < seg.node_count; j++) {   // j=1 to skip duplicated start
            total.route[total.node_count++] = seg.route[j];
        }
        total.total_distance += seg.total_distance;
        total.total_time    += seg.total_time;
        total.total_cost    += seg.total_cost;
    }

    // Last leg: wp[count-1] -> end, skip start to avoid duplicates
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

// ========== Toilet-Aware Path ==========
// Find nearest toilet from start, then go to end: start -> nearest toilet -> end
// Second leg can have constraint; overall fails if any segment fails
PathResult toiletAwarePath(
    int start, int end,
    int weight_type,
    bool has_constraint,
    int constraint_type,
    int constraint_limit
) {
    // Collect all cities with toilets
    static int toilets[N];
    int tcnt = 0;
    for (int i = 0; i < cityNum; i++) {
        if (wc[i]) {
            toilets[tcnt++] = i;
        }
    }

    // No toilets, go directly
    if (tcnt == 0)
    {
        if (has_constraint)
        {
            return constrainedShortestPath(start, end, weight_type,
                                           constraint_type, constraint_limit);
        }
        return shortestPath(start, end, weight_type);
    }

    // Run one Dijkstra to find nearest toilet
    static int dist[N], prev[N];
    static bool vis[N];
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

    // Find nearest reachable toilet
    int nearest = -1;
    for (int i = 0; i < tcnt; i++) {
        int t = toilets[i];
        if (dist[t] != INF && (nearest == -1 || dist[t] < dist[nearest])) {
            nearest = t;
        }
    }
    if (nearest == -1)   // all toilets unreachable
    {
        PathResult fail;
        fail.feasible = false;
        return fail;
    }

    // Reconstruct start->nearest path
    static int first[N];
    int fcnt = 0;
    for (int v = nearest; v != -1; v = prev[v]) {
        first[fcnt++] = v;
    }
    for (int i = 0; i < fcnt / 2; i++) {
        int t = first[i];
        first[i] = first[fcnt - 1 - i];
        first[fcnt - 1 - i] = t;
    }

    // Calculate constraint consumption of first leg
    int first_cons = 0;
    if (has_constraint)
    {
        for (int i = 0; i < fcnt - 1; i++) {
            first_cons += graph[constraint_type][first[i]][first[i + 1]];
        }
        if (first_cons > constraint_limit)   // first leg already exceeds limit
        {
            PathResult fail;
            fail.feasible = false;
            return fail;
        }
    }

    // Second leg: nearest -> end, deduct first leg constraint consumption
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

    // Concatenate: full first leg + second leg skipping start
    PathResult total;
    total.node_count = 0;
    total.feasible   = true;
    for (int j = 0; j < fcnt; j++) {
        total.route[total.node_count++] = first[j];
    }
    for (int j = 1; j < second.node_count; j++) {
        total.route[total.node_count++] = second.route[j];
    }

    // Accumulate three weights from final path
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

// ========== Dispatcher ==========
PathResult solve(const RouteRequest& request)
{
    int opt = (int)request.optimize;
    int ctype = (int)request.constraint_dim;

    if (request.need_toilet)
    {
        return toiletAwarePath(request.start_id, request.end_id,
                               opt, request.has_constraint,
                               ctype, request.constraint_limit);
    }
    if (!request.waypoints.empty())
    {
        return waypointPath(request.start_id, request.end_id,
                            request.waypoints.data(),
                            (int)request.waypoints.size(), opt);
    }
    if (request.has_constraint)
    {
        return constrainedShortestPath(request.start_id, request.end_id,
                                       opt, ctype, request.constraint_limit);
    }
    return shortestPath(request.start_id, request.end_id, opt);
}

// ========== Output ==========
void printPath(const PathResult& path)
{
    if (!path.feasible)
    {
        printf("No feasible path\n");
        return;
    }

    for (int i = 0; i < path.node_count; i++)
    {
        if (i > 0)
        {
            printf(" -> ");
        }
        printf("%s", id_to_name[path.route[i]].c_str());
    }
    printf("\nDistance: %d km  Time: %d h  Cost: %d yuan\n",
           path.total_distance, path.total_time, path.total_cost);
}
