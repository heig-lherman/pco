
#Qt version check
!contains(QT_VERSION, ^5\\.*\\..*) {
    message("Cannot build QtrainSim with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.0.")
}

# Compile without link to sound when loco have an accident
# CONFIG += NOSOUND

#Use maquettes instead of sim
#CONFIG += MAQUETTE

#Use cmain instead of cppmain
#CONFIG += CDEVELOP

#Use alternate painting style
#DEFINES += DRAW_BOUNDINGRECT

#Qt modules
QT += core gui widgets printsupport

!NOSOUND : QT += multimedia
!NOSOUND : DEFINES += WITHSOUND

#Target and template
TARGET = QtrainSim
TEMPLATE = app
DESTDIR = dist

#Configuration
CONFIG += debug


#Install the maquettes files
maquettes.path  =  $$OUT_PWD/dist/data/Maquettes
maquettes.files =  $$PWD/data/Maquettes/*
INSTALLS    += maquettes

#Install the file containing the infos about tracks
infos.path  =  $$OUT_PWD/dist/data
infos.files =  $$PWD/data/infosVoies.txt
INSTALLS    += infos

MAQUETTE: DEFINES += MAQUETTE
MAQUETTE: LIBS += -lmarklin

RESOURCES += $$PWD/qtrainsim.qrc

win32 {
    DEFINES += ON_WINDOWS
    UI_DIR = tmp/win/ui
    MOC_DIR = tmp/win/moc
    OBJECTS_DIR = tmp/win/obj
    RCC_DIR = tmp/win/rcc
    CDEVELOP: LIBS += -lpthread
}
unix {
    DEFINES += ON_LINUX
    UI_DIR = tmp/linux/ui
    MOC_DIR = tmp/linux/moc
    OBJECTS_DIR = tmp/linux/obj
    RCC_DIR = tmp/linux/rcc
}
macx {
    DEFINES += ON_MACOS
    UI_DIR = tmp/mac/ui
    MOC_DIR = tmp/mac/moc
    OBJECTS_DIR = tmp/mac/obj
    RCC_DIR = tmp/mac/rcc
    # Avoid to create a bundle that causes troubles with the maquette files
    CONFIG -= app_bundle
}

CDEVELOP: DEFINES += CDEVELOP

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/voie.cpp \
    $$PWD/src/voiedroite.cpp \
    $$PWD/src/voiecourbe.cpp \
    $$PWD/src/voieaiguillage.cpp \
    $$PWD/src/voiecroisement.cpp \
    $$PWD/src/voievariable.cpp \
    $$PWD/src/voiebuttoir.cpp \
    $$PWD/src/voietraverseejonction.cpp \
    $$PWD/src/simview.cpp \
    $$PWD/src/commandetrain.cpp \
    $$PWD/src/loco.cpp \
    $$PWD/src/contact.cpp \
    $$PWD/src/segment.cpp \
    $$PWD/src/trainsimsettings.cpp \
    $$PWD/src/maquettemanager.cpp \
    $$PWD/src/voieaiguillageenroule.cpp \
    $$PWD/src/voieaiguillagetriple.cpp \
    $$PWD/src/ctrain_handler.cpp

HEADERS += \
    $$PWD/src/mainwindow.h \
    $$PWD/src/voie.h \
    $$PWD/src/voiedroite.h \
    $$PWD/src/voiecourbe.h \
    $$PWD/src/voieaiguillage.h \
    $$PWD/src/voiecroisement.h \
    $$PWD/src/voievariable.h \
    $$PWD/src/voiebuttoir.h \
    $$PWD/src/voietraverseejonction.h \
    $$PWD/src/simview.h \
    $$PWD/src/connect.h \
    $$PWD/src/commandetrain.h \
    $$PWD/src/general.h \
    $$PWD/src/loco.h \
    $$PWD/src/contact.h \
    $$PWD/src/segment.h \
    $$PWD/src/trainsimsettings.h \
    $$PWD/src/maquettemanager.h \
    $$PWD/src/voieaiguillageenroule.h \
    $$PWD/src/voieaiguillagetriple.h \
    $$PWD/src/ctrain_handler.h

OTHER_FILES += $$PWD/data/infosVoies.txt
