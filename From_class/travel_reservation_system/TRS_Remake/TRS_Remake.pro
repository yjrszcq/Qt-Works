QT       += core gui
QT       += sql
QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adddialog.cpp \
    bus.cpp \
    flight.cpp \
    hotel.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    mysqldb.cpp \
    reservation.cpp \
    server.cpp \
    setdbdialog.cpp \
    signupdialog.cpp \
    user.cpp

HEADERS += \
    adddialog.h \
    bus.h \
    flight.h \
    hotel.h \
    logindialog.h \
    mainwindow.h \
    mysqldb.h \
    reservation.h \
    server.h \
    setdbdialog.h \
    signupdialog.h \
    user.h

FORMS += \
    adddialog.ui \
    mainwindow.ui

TRANSLATIONS += \
    TRS_Remake_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
