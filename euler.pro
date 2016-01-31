TEMPLATE = app
TARGET = euler
VERSION = 1.0.0
QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4) {
           QT += widgets
}

#QMAKE_CXXFLAGS = -Wno-unused-parameter
#CONFIG += console
DEPENDPATH += . src
INCLUDEPATH += . src

HEADERS += src/Canvas.h src/Form.h
FORMS += src/Form.ui
SOURCES += src/Canvas.cpp src/Form.cpp src/main.cpp

#RESOURCES += src/resources.qrc
#RC_FILE += src/euler.rc
