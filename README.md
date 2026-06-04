# RouteFinder - 城市路径规划工具

基于 Dijkstra 算法的命令行路径规划工具，支持多种优化目标、约束条件、途经点和厕所优先路径。

## 城市数据

| 城市 | 英文名 | 有无厕所 |
|------|--------|----------|
| 西安 | Xi'an | 无 |
| 北京 | Beijing | 有 |
| 上海 | Shanghai | 无 |
| 成都 | Chengdu | 有 |
| 武汉 | Wuhan | 无 |

## 快速开始

```cmd
git clone https://github.com/Enkrid-Rao/RouterFinder.git
cd RouterFinder
run.bat --srt Xi'an --dst Beijing --t
```

`build/` 目录已包含编译好的 `RouteFinder.exe`，无需安装任何工具链即可运行。

## 命令行参数

### 基础参数

| 参数 | 说明 | 示例 |
|------|------|------|
| `--srt <城市>` | 起点城市 | `--srt Xi'an` |
| `--dst <城市>` | 终点城市 | `--dst Shanghai` |
| `--t` | 优化时间（默认） | `--t` |
| `--d` | 优化路程 | `--d` |
| `--c` | 优化费用 | `--c` |

### 约束参数

在某维度不超过限制值的前提下，最小化另一维度：

| 参数 | 约束维度 | 优化维度 | 示例 |
|------|----------|----------|------|
| `--Td <限制>` | 时间 | 路程 | `--Td 8` |
| `--Tc <限制>` | 时间 | 费用 | `--Tc 10` |
| `--Dt <限制>` | 路程 | 时间 | `--Dt 2000` |
| `--Dc <限制>` | 路程 | 费用 | `--Dc 1500` |
| `--Ct <限制>` | 费用 | 时间 | `--Ct 600` |
| `--Cd <限制>` | 费用 | 路程 | `--Cd 500` |

### 特殊功能

| 参数 | 说明 |
|------|------|
| `--wp <城市>` | 途经点，可多次使用来指定多个途经点 |
| `--wc` | 厕所优先模式，路径会经过最近的厕所再前往终点 |

## 使用示例

```cmd
# 基础：西安到北京，时间最短
run.bat --srt Xi'an --dst Beijing --t

# 约束：8小时内从西安到上海，路程最短
run.bat --srt Xi'an --dst Shanghai --Td 8

# 途经点：北京到上海，经过武汉，路程最短
run.bat --srt Beijing --dst Shanghai --wp Wuhan --d

# 厕所优先：西安到上海，优先经过厕所
run.bat --srt Xi'an --dst Shanghai --wc --t

# 费用约束+厕所优先：500元内从成都到上海，时间最短
run.bat --srt Chengdu --dst Shanghai --wc --Ct 500
```

## 输出示例

```
Xi'an -> Chengdu -> Shanghai
路程: 2700 km  时间: 12 h  费用: 1100 yuan
```

无可行路径时输出：

```
无可行路径
```

## 算法

- **最短路径** — 标准 Dijkstra O(N²)
- **约束最短路径** — Label-setting + Pareto 支配剪枝
- **途经点路径** — 分段 Dijkstra 拼接
- **厕所优先路径** — 先 Dijkstra 找最近厕所，再前往终点（第二段可带约束）

## 开发编译

需要 CMake + g++（MinGW）：

```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

或在 CLion 中直接打开项目文件夹，CMakeLists.txt 会被自动识别。

## 项目结构

```
RouteFinder/
├── main.cpp              # 入口
├── CMakeLists.txt        # CMake 配置
├── run.bat               # 运行脚本
├── data.csv              # 城市路线数据
├── wc.csv                # 城市厕所数据
├── build/
│   └── RouteFinder.exe   # 预编译可执行文件
├── include/
│   ├── commandParse.h    # 命令行解析
│   ├── data.h            # 数据结构
│   ├── Dijkstra.h        # 算法声明
│   └── io.h              # 控制台编码设置
└── src/
    ├── commandParse.cpp  # C++ 风格命令行解析
    ├── data.cpp          # CSV 数据读取
    ├── Dijkstra.cpp      # 核心算法实现
    └── io.cpp            # UTF-8 控制台编码
```

## 编码说明

- 源文件：UTF-8
- 数据文件：UTF-8 无 BOM
- 控制台输出：`SetConsoleOutputCP(CP_UTF8)` + `chcp 65001`
- 输出使用 `std::cout` 确保跨终端兼容
