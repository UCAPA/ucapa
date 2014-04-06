#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T17:50:41
#
#-------------------------------------------------

TARGET = Derivate
TEMPLATE = app

CONFIG += console \
          c++11
CONFIG -= app_bundle \
          qt

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
    LIBS += -I/usr/local/include  -pthread
    LIBS += -L/usr/local/lib -lavformat -lavcodec -lva -lz -lswscale -lavutil
#    LIBS += -I/usr/local/include  -pthread -L/usr/local/lib -lavformat -lavcodec -ldl -lvdpau -lva -lX11 -lasound -lSDL -lz -lswscale -lavutil -lm
}

SOURCES += main.cpp \
    extnavdata.cpp \
    extardrone.cpp


HEADERS  += \
    extnavdata.h \
    extardrone.h

