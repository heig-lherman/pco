QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lpcosynchro

SOURCES += \
    display.cpp \
    extractor.cpp \
    factory.cpp \
    main.cpp \
    mainwindow.cpp \
    seller.cpp \
    utils.cpp \
    wholesale.cpp \
    windowinterface.cpp

HEADERS += \
    costs.h \
    display.h \
    extractor.h \
    factory.h \
    mainwindow.h \
    seller.h \
    utils.h \
    wholesale.h \
    windowinterface.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    images/copper.png \
    images/funds_color.png \
    images/sand.png \
    images/Microchips.png \
    images/Service_bots.png \
    images/station-essence.png \
    images/Plastics.png \
    images/copper_scaled.png \
    images/factory_scaled.png \
    images/petrol_scaled.png \
    images/warehouse_scaled.png \
    images/sand_scaled.png \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
