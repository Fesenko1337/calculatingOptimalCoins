QT += core xml testlib
QT -= gui

CONFIG += qt warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_tests

# Пути к заголовочным файлам
INCLUDEPATH += ../calculatingOptimalCoins

# Исходные файлы
SOURCES += tst_tests.cpp \
    ../calculatingOptimalCoins/coinSolver.cpp \
    ../calculatingOptimalCoins/xmlParser.cpp \
    ../calculatingOptimalCoins/fileHandler.cpp \
    ../calculatingOptimalCoins/error.cpp

# Заголовочные файлы
HEADERS += \
    ../calculatingOptimalCoins/coinSolver.h \
    ../calculatingOptimalCoins/xmlParser.h \
    ../calculatingOptimalCoins/fileHandler.h \
    ../calculatingOptimalCoins/error.h

# Генерировать moc-файлы в папке tests/
MOC_DIR = .
