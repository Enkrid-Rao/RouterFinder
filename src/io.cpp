//
// Created by raoxi on 2026/6/4.
//

#include "include/io.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

void setChineseIO()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    _setmode((stdout)->_file, _O_BINARY);  // 修复 cmd.exe 下 printf 无输出的问题
}
