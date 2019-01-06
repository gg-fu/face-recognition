#-------------------------------------------------
#
# Project created by QtCreator 2018-11-10T14:31:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Camera
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    video.cpp

HEADERS += \
        mainwindow.h \
    video.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv \
               /usr/local/include/opencv2\
               /usr/local/include\
               /usr/local/jpeglib/include

LIBS += /usr/local/lib/libopencv_core.so
LIBS += /usr/local/lib/libopencv_highgui.so
LIBS += /usr/local/lib/libopencv_core.so
LIBS += /usr/local/lib/libopencv_highgui.so
LIBS += /usr/local/lib/libopencv_calib3d.so
LIBS += /usr/local/lib/libopencv_features2d.so
LIBS += /usr/local/lib/libopencv_flann.so
LIBS += /usr/local/lib/libopencv_imgcodecs.so
LIBS += /usr/local/lib/libopencv_imgproc.so
LIBS += /usr/local/lib/libopencv_ml.so
LIBS += /usr/local/lib/libopencv_objdetect.so
LIBS += /usr/local/lib/libopencv_photo.so
LIBS += /usr/local/lib/libopencv_video.so
LIBS += /usr/local/lib/libopencv_shape.so
LIBS += /usr/local/lib/libopencv_videoio.so
LIBS += /usr/local/lib/libopencv_stitching.so
LIBS += /usr/local/lib/libopencv_superres.so
LIBS += /usr/local/jpeglib/lib/libjpeg.so



