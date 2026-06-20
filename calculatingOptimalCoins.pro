QT += core xml
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
        coinSolver.cpp \
        error.cpp \
        fileHandler.cpp \
        main.cpp \
        xmlParser.cpp


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    coinSolver.h \
    error.h \
    fileHandler.h \
    xmlParser.h
