QT += core gui quick testlib
CONFIG += testcase c++17
TEMPLATE = app
TARGET = tst_omawrite

INCLUDEPATH += ../src
SOURCES += \
    tst_omawrite.cpp \
    ../src/backend.cpp \
    ../src/markdownhighlighter.cpp \
    ../src/previewdocument.cpp
HEADERS += \
    ../src/backend.h \
    ../src/markdownhighlighter.h \
    ../src/previewdocument.h

QT += widgets printsupport quickcontrols2 quickdialogs2 dbus
