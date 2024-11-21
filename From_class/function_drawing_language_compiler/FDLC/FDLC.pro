QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codeeditor.cpp \
    compiler.cpp \
    drawwidget.cpp \
    globaldefines.cpp \
    linenumberarea.cpp \
    main.cpp \
    mainwindow.cpp \
    parsers.cpp \
    scanner.cpp

HEADERS += \
    codeeditor.h \
    compiler.h \
    drawwidget.h \
    globaldefines.h \
    linenumberarea.h \
    mainwindow.h \
    parsers.h \
    scanner.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    FDLC_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
