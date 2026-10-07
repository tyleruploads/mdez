cmake_minimum_required(VERSION 3.16)
project(mdez VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(mdez
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
)

target_link_libraries(mdez PRIVATE Qt6::Widgets)
