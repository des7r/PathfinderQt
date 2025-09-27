cmake_minimum_required(VERSION 3.16)

project(PathfinderQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    main.cpp
    mainwindow.cpp
    gridscene.cpp
    pathfinder.cpp
)

set(HEADERS
    mainwindow.h
    gridscene.h
    gridview.h
    pathfinder.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
