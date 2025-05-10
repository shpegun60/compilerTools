INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include(gnu/gnumapfile.pri)
include(clang/clangmapfile.pri)

HEADERS += \
    $$PWD/imapfile.h \
    $$PWD/imapfilereader.h

SOURCES += \
    $$PWD/imapfile.cpp
