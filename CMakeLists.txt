cmake_minimum_required(VERSION 3.16)
project(bmp_editor)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(bmp_editor
        main.cpp
        Image.cpp
        ImageParser.cpp
        Filter.cpp
        FilterFactory.cpp
        )

add_catch(test_parser test_parser.cpp ImageParser.cpp)

