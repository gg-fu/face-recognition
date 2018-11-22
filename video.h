
#ifndef _VIDEO_H
#define _VIDEO_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "jpeglib.h"
#include <iostream>
#include <opencv2/highgui.hpp>
using namespace cv;

typedef struct {
    uint8_t* start;
    size_t length;
} buffer_t;

typedef struct {
    int fd;
    uint32_t width;
    uint32_t height;
    size_t buffer_count;
    buffer_t* buffers;
    buffer_t head;
} camera_t;

class CCamera
{
    public:
        CCamera();
        ~CCamera();
        camera_t* CCamera_open(const char * device, uint32_t width, uint32_t height);
        void CCamera_init(camera_t* camera);
        void CCamera_start(camera_t* camera);
        void CCamera_stop(camera_t* camera);
        void CCamera_finish(camera_t* camera);
        void CCamera_close(camera_t* camera);
        int CCamera_capture(camera_t* camera);
        camera_t* CCamera_OPEN(const char * device,uint32_t width, uint32_t height);
        int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);

    private:
        uint8_t* yuyv2rgb(uint8_t* yuyv, uint32_t width, uint32_t height);
        void rgb2jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality);
        int xioctl(int fd, int request, void* arg);
        int convert_yuv_to_rgb_pixel(int y, int u, int v);
        void quit(const char * msg){
            fprintf(stderr, "[%s] %d: %s\n", msg, errno, strerror(errno));
            exit(EXIT_FAILURE);}
        int minmax(int min, int v, int max){
            return (v < min) ? min : (max < v) ? max : v;
        }



};



#endif
