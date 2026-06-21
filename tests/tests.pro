QT += core testlib xml
CONFIG += c++17 console testcase
CONFIG -= app_bundle

TARGET = tst_tests


INCLUDEPATH += $$PWD/..


SOURCES += \
    tst_tests.cpp \
    ../coinSolver.cpp \
    ../error.cpp \
    ../fileHandler.cpp \
    ../xmlParser.cpp

HEADERS += \
    ../coinSolver.h \
    ../error.h \
    ../fileHandler.h \
    ../xmlParser.h
