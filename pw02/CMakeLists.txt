cmake_minimum_required(VERSION 3.16)
project(PCO_Labo_2 VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

add_executable(PCO_Labo_2 WIN32 MACOSX_BUNDLE
        src/main.cpp
        src/mainwindow.cpp src/mainwindow.h
        src/mythread.cpp src/mythread.h
        src/threadmanager.cpp src/threadmanager.h
        ui/mainwindow.ui
)
target_link_libraries(PCO_Labo_2 PRIVATE
        Qt::Concurrent
        Qt::Core
        Qt::Gui
        pcosynchro
)

if ((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(PCO_Labo_2 PRIVATE
            Qt::Widgets
    )
endif ()

install(TARGETS PCO_Labo_2
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
