cmake_minimum_required(VERSION 3.16)
project(Lab3_Factory VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
include(GNUInstallDirs)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

add_executable(Lab3_Factory WIN32 MACOSX_BUNDLE
        images.qrc
        costs.h
        display.cpp display.h
        extractor.cpp extractor.h
        factory.cpp factory.h
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        seller.cpp seller.h
        utils.cpp utils.h
        wholesale.cpp wholesale.h
        windowinterface.cpp windowinterface.h
)
target_link_libraries(Lab3_Factory PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        pcosynchro
)

if ((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(Lab3_Factory PRIVATE
            Qt::Widgets
    )
endif ()

install(TARGETS Lab3_Factory
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
