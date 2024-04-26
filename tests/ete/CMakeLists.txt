cmake_minimum_required(VERSION 3.8)

project(Ete LANGUAGES CXX)

add_executable(ete ete.cpp  ../../geometry/src/point.cpp ../../geometry/src/plane.cpp ../../geometry/src/line.cpp ../../geometry/src/vector.cpp
                            ../../geometry/src/triangle.cpp ../../geometry/src/octree.cpp ../../geometry/src/intersection.cpp
                            ../../geometry/src/double_numbers.cpp)