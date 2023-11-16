cmake_minimum_required(VERSION 3.16)
project(QTrainSim VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
include(GNUInstallDirs)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui PrintSupport Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Multimedia)

add_executable(QTrainSim WIN32 MACOSX_BUNDLE
        QtrainSim/qtrainsim.qrc
        QtrainSim/src/commandetrain.cpp QtrainSim/src/commandetrain.h
        QtrainSim/src/connect.h
        QtrainSim/src/contact.cpp QtrainSim/src/contact.h
        QtrainSim/src/ctrain_handler.cpp QtrainSim/src/ctrain_handler.h
        QtrainSim/src/general.h
        QtrainSim/src/loco.cpp QtrainSim/src/loco.h
        QtrainSim/src/main.cpp
        QtrainSim/src/mainwindow.cpp QtrainSim/src/mainwindow.h
        QtrainSim/src/maquettemanager.cpp QtrainSim/src/maquettemanager.h
        QtrainSim/src/segment.cpp QtrainSim/src/segment.h
        QtrainSim/src/simview.cpp QtrainSim/src/simview.h
        QtrainSim/src/trainsimsettings.cpp QtrainSim/src/trainsimsettings.h
        QtrainSim/src/voie.cpp QtrainSim/src/voie.h
        QtrainSim/src/voieaiguillage.cpp QtrainSim/src/voieaiguillage.h
        QtrainSim/src/voieaiguillageenroule.cpp QtrainSim/src/voieaiguillageenroule.h
        QtrainSim/src/voieaiguillagetriple.cpp QtrainSim/src/voieaiguillagetriple.h
        QtrainSim/src/voiebuttoir.cpp QtrainSim/src/voiebuttoir.h
        QtrainSim/src/voiecourbe.cpp QtrainSim/src/voiecourbe.h
        QtrainSim/src/voiecroisement.cpp QtrainSim/src/voiecroisement.h
        QtrainSim/src/voiedroite.cpp QtrainSim/src/voiedroite.h
        QtrainSim/src/voietraverseejonction.cpp QtrainSim/src/voietraverseejonction.h
        QtrainSim/src/voievariable.cpp QtrainSim/src/voievariable.h
        src/cppmain.cpp
        src/launchable.h
        src/locomotive.cpp src/locomotive.h
        src/locomotivebehavior.cpp src/locomotivebehavior.h
        src/synchro.h
        src/synchrointerface.h
        src/route.h src/route.cpp
)
target_include_directories(QTrainSim PRIVATE
        QtrainSim/src
)

target_link_libraries(QTrainSim PRIVATE
        Qt::Core
        Qt::Gui
        Qt::PrintSupport
        Qt::Widgets
        pcosynchro
)


# Resources:
set(qtrainsim_resource_files
        "QtrainSim/images/explosion.png"
        "QtrainSim/images/simulate_break.png"
        "QtrainSim/images/simulate_end.png"
        "QtrainSim/images/simulate_msteps.png"
        "QtrainSim/images/simulate_msteps_update.png"
        "QtrainSim/images/simulate_reset.png"
        "QtrainSim/images/simulate_restart.png"
        "QtrainSim/images/simulate_run.png"
        "QtrainSim/images/simulate_start.png"
        "QtrainSim/images/simulate_step.png"
        "QtrainSim/images/zoomin.png"
        "QtrainSim/images/zoomout.png"
        "QtrainSim/sound/explosion.wav"
)

if (NOT NOSOUND)
    target_compile_definitions(QTrainSim PRIVATE
            WITHSOUND
    )

    target_link_libraries(QTrainSim PRIVATE
            Qt::Multimedia
    )
endif ()

if (MAQUETTE)
    target_compile_definitions(QTrainSim PRIVATE
            MAQUETTE
    )

    target_link_libraries(QTrainSim PRIVATE
            marklin
    )
endif ()

if (WIN32)
    target_compile_definitions(QTrainSim PRIVATE
            ON_WINDOWS
    )
endif ()

if (CDEVELOP AND WIN32)
    target_link_libraries(QTrainSim PRIVATE
            pthread
    )
endif ()

if (UNIX)
    target_compile_definitions(QTrainSim PRIVATE
            ON_LINUX
    )
endif ()

if (MACOS)
    set_target_properties(QTrainSim PROPERTIES
            MACOSX_BUNDLE FALSE
    )
    target_compile_definitions(QTrainSim PRIVATE
            ON_MACOS
    )
endif ()

if (CDEVELOP)
    target_compile_definitions(QTrainSim PRIVATE
            CDEVELOP
    )
endif ()

install(TARGETS QTrainSim
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
