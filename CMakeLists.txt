cmake_minimum_required(VERSION 3.28)
project(Astar)

add_executable(Astar main.cpp)
target_link_libraries(Astar raylib winmm)
target_include_directories(Astar PRIVATE SYSTEM raylib-5.0_win64_msvc16/include)
target_link_directories(Astar PRIVATE raylib-5.0_win64_msvc16/lib)