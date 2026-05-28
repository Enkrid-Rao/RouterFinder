#include "include/data.h"
#include "include/commandParse.h"
#include "include/Dijkstra.h"
#include <iostream>

int main(int argc, char* argv[]) {
    readWc();
    readEdge();

    auto req = parseCommandLine(argc, argv);
    if (!req.has_value()) {
        return 1;
    }

    auto result = solve(req.value());
    printPath(result);

    return 0;
}
