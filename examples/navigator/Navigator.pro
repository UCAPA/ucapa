#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T17:50:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Navigator
TEMPLATE = app

CONFIG += c++11

DESTDIR = $$_PRO_FILE_PWD_

DEFINES += ASIO_STANDALONE

INCLUDEPATH +=  \
                 ../../include \
                ../../external/asio/include/ \
                ../../external/ffmpeg/include/

LIBS += -L"$$_PRO_FILE_PWD_/../../lib/" -lucapa

win32 {
    LIBS += -lws2_32 -lmswsock
    LIBS += -L"$$_PRO_FILE_PWD_/../../external/ffmpeg/lib/" -lavformat -lavcodec -lswscale -lavutil
}
unix {
    LIBS += -L/usr/local/lib -lavformat -lavcodec -lva -lz -lswscale -lavutil
#    LIBS += -I/usr/local/include  -pthread -L/usr/local/lib -lavformat -lavcodec -ldl -lvdpau -lva -lX11 -lasound -lSDL -lz -lswscale -lavutil -lm
}

SOURCES += main.cpp\
    mainwindow.cpp \
    qdronedialog.cpp

HEADERS  += \
    mainwindow.h \
    qdronedialog.h

FORMS    += mainwindow.ui \
    qdronedialog.ui
