QT += core xml
QT -= gui

CONFIG += c++11

TEMPLATE = app
TARGET = calculatingOptimalCoins

SOURCES += \
    main.cpp \
    coinSolver.cpp \
    error.cpp \
    fileHandler.cpp \
    xmlParser.cpp

HEADERS += \
    coinSolver.h \
    error.h \
    fileHandler.h \
    xmlParser.h
