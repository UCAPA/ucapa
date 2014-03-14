#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T17:50:41
#
# This project build only the UCAPA lib.
#
#-------------------------------------------------

TARGET = ucapa
TEMPLATE = lib

CONFIG += c++11 \
          staticlib

CONFIG -= app_bundle \
          qt

DESTDIR = $$_PRO_FILE_PWD_/lib


DEFINES += ASIO_STANDALONE

# As our dependencies are not too big, we put them into an external/ subdirectory.
INCLUDEPATH +=  ./include \
                ./external/asio/include/ \
                ./external/FFmpeg/include/
win32 {
    LIBS += -lws2_32 -lmswsock
    LIBS += -L"$$_PRO_FILE_PWD_/external/FFmpeg/lib/" -lavformat -lavcodec -lswscale -lavutil
}
unix {
    LIBS += -L/usr/local/lib -lavformat -lavcodec -lva -lz -lswscale -lavutil
#    LIBS += -I/usr/local/include  -pthread -L/usr/local/lib -lavformat -lavcodec -ldl -lvdpau -lva -lX11 -lasound -lSDL -lz -lswscale -lavutil -lm
}


SOURCES += \
    src/ardrone.cpp \
    src/ardroneconnections.cpp \
    src/vector3.cpp \
    src/navdata.cpp \
    src/quaternion.cpp \
    src/video.cpp

HEADERS  += \
    include/vector3.h \
    include/ardroneconnections.h \
    include/ardrone.h \
    include/navdata.h \
    include/utils.h \
    include/matrix.h \
    include/quaternion.h \
    include/video.h
