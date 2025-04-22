INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(gnu/gnulinker.pri)
include(clang/clanglinker.pri)

HEADERS += \
    $$PWD/linkerfile.h

SOURCES += \
    $$PWD/linkerfile.cpp
