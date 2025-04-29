INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(gnu/gnulinker.pri)
include(clang/clanglinker.pri)

HEADERS += \
    $$PWD/ilinkerfile.h \
    $$PWD/ilinkerreader.h \
    $$PWD/linkerfilefactory.h \
    $$PWD/linkerreaderfactory.h

SOURCES += \
    $$PWD/ilinkerfile.cpp \
    $$PWD/linkerfilefactory.cpp \
    $$PWD/linkerreaderfactory.cpp
