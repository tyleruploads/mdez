cmake_minimum_required(VERSION 3.16)
project(mdez VERSION 0.8.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Svg)

qt_standard_project_setup()

qt_add_executable(mdez
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
)

if(OUTPUT_NAME)
    set_target_properties(mdez PROPERTIES OUTPUT_NAME "${OUTPUT_NAME}")
endif()

target_link_libraries(mdez PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Svg)

target_compile_definitions(mdez PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_command(TARGET mdez POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/media"
    "${CMAKE_BINARY_DIR}/media"
)
