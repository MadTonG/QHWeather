#-------------------------------------------------
#
# Project created by QtCreator 2017-03-27T03:30:41
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QHWeather
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    nwsendpoints.cpp \
    downloader.cpp \
    appoptions.cpp \
    conusradar.cpp \
    alertswindow.cpp \
    ridgeradarlite.cpp \
    locationeditor.cpp

HEADERS  += mainwindow.h \
    nwsendpoints.h \
    downloader.h \
    appoptions.h \
    conusradar.h \
    alertswindow.h \
    ridgeradarlite.h \
    locationeditor.h

FORMS    += mainwindow.ui \
    appoptions.ui \
    conusradar.ui \
    alertswindow.ui \
    ridgeradarlite.ui \
    locationeditor.ui

RESOURCES += \
    resources.qrc

RC_FILE = NWS_resource.rc
