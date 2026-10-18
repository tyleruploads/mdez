cmake_minimum_required(VERSION 3.16)
project(mdez VERSION 0.10.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

set(OPTIONAL_LIBS "")
if(NOT EMSCRIPTEN)
    find_package(Qt6 COMPONENTS PrintSupport)
    if(TARGET Qt6::PrintSupport)
        list(APPEND OPTIONAL_LIBS Qt6::PrintSupport)
    endif()
endif()

qt_standard_project_setup()

qt_add_executable(mdez
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/resources.qrc
)

if(OUTPUT_NAME)
    set_target_properties(mdez PROPERTIES OUTPUT_NAME "${OUTPUT_NAME}")
endif()

target_link_libraries(mdez PRIVATE Qt6::Widgets Qt6::Svg ${OPTIONAL_LIBS})

target_compile_definitions(mdez PRIVATE
    APP_VERSION="${PROJECT_VERSION}"
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if(NOT EMSCRIPTEN)
    add_custom_command(TARGET mdez POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/media"
        "${CMAKE_BINARY_DIR}/media"
    )
endif()

if(EMSCRIPTEN)
    target_link_options(mdez PRIVATE 
        "-sSINGLE_FILE=1"
        "-sASYNCIFY=1"
    )
endif()
