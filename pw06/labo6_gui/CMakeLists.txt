cmake_minimum_required(VERSION 3.5)

project(PCO_lab06_gui)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core Gui Widgets Test Svg REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    gui/src/arrowitem.cpp
    gui/src/computeenginegui.cpp
    gui/src/computeenvironmentgui.cpp
    gui/src/mainwindow.cpp
    gui/src/guiinterface.cpp
    gui/src/main.cpp
    gui/src/simview.cpp
)

set(HEADERS
    gui/src/connect.h
    gui/src/guiinterface.h
    gui/src/messages.h
    gui/src/simview.h
    gui/src/arrowitem.h
    gui/src/computeenginegui.h
    gui/src/computeenvironmentgui.h
    gui/src/mainwindow.h
)

add_executable(PCO_lab06_gui ${SOURCES} ${HEADERS})

target_link_libraries(PCO_lab06_gui PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test Qt5::Svg -lpcosynchro labo6_lib)

file(COPY gui/images/ DESTINATION ${CMAKE_BINARY_DIR}/images/)
