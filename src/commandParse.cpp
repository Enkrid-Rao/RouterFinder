#include "include/commandParse.h"
#include "include/data.h"
#include <iostream>
#include <string>
#include <cstring>

static bool isFlag(const char* arg) {
    return arg[0] == '-' && arg[1] == '-';
}

auto parseCommandLine(int argc, char* argv[]) -> std::optional<RouteRequest> {
    
}
