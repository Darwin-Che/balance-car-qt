QT       += core gui
QT += bluetooth
QT += charts

QMAKE_INFO_PLIST = Info.plist

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    linechart.cpp \
    linechartmulti.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp

HEADERS += \
    imu.h \
    linechart.h \
    linechartmulti.h \
    mainwindow.h \
    model.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    balance-car-qt_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Info.plist \
    Info_copy.plist
