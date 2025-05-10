QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QT += core gui widgets qml


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(mapfile/mapfile.pri)
include(linkerscript/linkerscript.pri)


SOURCES += \
    hashindex.cpp \
    main.cpp \
    mainwindow.cpp \
    mapfile.cpp \
    segment.cpp \
    symbol.cpp

HEADERS += \
    hashindex.h \
    macro.h \
    mainwindow.h \
    mapfile.h \
    segment.h \
    symbol.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
