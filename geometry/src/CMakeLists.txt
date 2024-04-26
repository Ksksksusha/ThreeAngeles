cmake_minimum_required(VERSION 3.8)

project(triangles LANGUAGES CXX)

add_executable(triag plane.cpp vector.cpp line.cpp segment.cpp triangle.cpp ../main.cpp)