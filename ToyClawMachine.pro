TEMPLATE = app
CONFIG += c++11 no_keywords console

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp  \
    ApplicationController.cpp

RESOURCES += qml.qrc

HEADERS += \
    ApplicationController.h
