TEMPLATE = app
CONFIG += c++11 no_keywords console

QT += qml quick
QT += multimedia
CONFIG += c++11
CONFIG += use_sim

SOURCES += main.cpp  \
    ApplicationController.cpp \
    MainProcess.cpp

HEADERS += \
    ApplicationController.h \
    MainProcess.h

use_sim {
DEFINES += APP_SIM
SOURCES += ApplicationSim.cpp
HEADERS += ApplicationSim.h
}
use_stm32 {
DEFINES += APP_STM32
SOURCES += ApplicationSTM32.cpp
HEADERS += ApplicationSTM32.h
}
RESOURCES += qml.qrc

