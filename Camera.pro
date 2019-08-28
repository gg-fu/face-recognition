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
        ./src/main.cpp \
        ./src/mainwindow.cpp \
    	./src/video.cpp\
	./src/HAL_TCP_linux.c\
	./src/queue.cpp\
	./src/pic_thread.cpp\
	./src/rtp_client.cpp\
	./src/mat_to_jpeg.cpp

HEADERS += \
        ./include/mainwindow.h \
        ./include/video.h\
	./include/HAL_TCP_linux.h\
	./include/pic_thread.h\
	./include/queue.h\
	./include/rtp_client.h\
	./include/mat_to_jpeg.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += /usr/local/opencv-arm/include/opencv \
               /usr/local/opencv-arm/include/opencv2\
               /usr/local/opencv-arm/include\
               /usr/local/jpeglib/include\
	       /home/bruvin_lu/jrtplib-3.11.1/src

LIBS += -L /usr/local/opencv-arm/lib/
LIBS += -lopencv_core
LIBS += -lopencv_highgui
LIBS += -lopencv_calib3d
LIBS += -lopencv_features2d
LIBS += -lopencv_flann
LIBS += -lopencv_imgcodecs
LIBS += -lopencv_imgproc
LIBS += -lopencv_ml
LIBS += -lopencv_objdetect
LIBS += -lopencv_photo
LIBS += -lopencv_video
LIBS += -lopencv_shape
LIBS += -lopencv_videoio
LIBS += -lopencv_stitching
LIBS += -lopencv_superres
LIBS += -L/usr/local/jpeglib/lib/
LIBS += -ljpeg
LIBS += -L/home/bruvin_lu/Camera_new/lib/
LIBS += -lncnn
LIBS += -L/usr/local/JRTPLib
LIBS += -ljthread
LIBS += -ljrtp

