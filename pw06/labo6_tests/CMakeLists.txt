cmake_minimum_required(VERSION 3.5)

project(PCO_lab06_tests)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CONSOLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(CONSOLE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pcotest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/testcomputengine.h
)

add_executable(PCO_lab06_tests ${CONSOLE_SOURCES} ${CONSOLE_HEADERS})

target_link_libraries(PCO_lab06_tests PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test -lpcosynchro labo6_lib gtest)

