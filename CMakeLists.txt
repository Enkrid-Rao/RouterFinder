cmake_minimum_required(VERSION 3.16)
project(RouteFinder VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========== Source files ==========
set(SOURCES
    main.cpp
    src/data.cpp
    src/commandParse.cpp
    src/Dijkstra.cpp
    src/io.cpp
)

set(HEADERS
    include/data.h
    include/commandParse.h
    include/Dijkstra.h
    include/io.h
)

# ========== Executable ==========
add_executable(RouteFinder ${SOURCES} ${HEADERS})

target_include_directories(RouteFinder PRIVATE include src .)

# ========== Compile options ==========
if(MSVC)
    target_compile_options(RouteFinder PRIVATE /W4)
else()
    target_compile_options(RouteFinder PRIVATE -Wall -Wextra)
endif()

# ========== Copy data files to exe output directory ==========
add_custom_command(TARGET RouteFinder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/data.csv
        $<TARGET_FILE_DIR:RouteFinder>/data.csv
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/wc.csv
        $<TARGET_FILE_DIR:RouteFinder>/wc.csv
    COMMENT "Copying data files to output dir"
)
