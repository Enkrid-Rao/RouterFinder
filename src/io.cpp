//
// Created by raoxi on 2026/6/4.
//

#include "include/io.h"

#ifdef _WIN32
#include <windows.h>
#endif

void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}
