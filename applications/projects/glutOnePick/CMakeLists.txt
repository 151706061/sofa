cmake_minimum_required(VERSION 3.22)
project(glutOnePick)

find_package(SofaSimpleGUI)
find_package(SofaOpenglVisual REQUIRED)

add_executable(${PROJECT_NAME} glutOnePick.cpp oneTetra.h)
target_link_libraries(${PROJECT_NAME} SofaSimpleGUI SofaOpenglVisual)
