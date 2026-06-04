#include "include/data.h"
#include "include/commandParse.h"
#include "include/Dijkstra.h"
#include "include/io.h"
#include <iostream>

int main(int argc, char* argv[]) {
    setChineseIO();
    readWc();
    readEdge();

    RouteRequest req;
    if (!parseCommandLine(argc, argv, req)) {
        return 1;
    }

    PathResult result = solve(req);
    printPath(result);

    return 0;
}
