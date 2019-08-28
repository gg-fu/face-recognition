#include "include/video.h"

CCamera::CCamera()
{

}

CCamera::~CCamera()
{

}

int CCamera::xioctl(int fd, int request, void* arg)
{
  for (int i = 0; i < 100; i++) {
    int r = ioctl(fd, request, arg);
    if (r != -1 || errno != EINTR) return r;
  }
  return -1;
}

camera_t* CCamera::CCamera_open(const char * device, uint32_t width, uint32_t height)
{
    int fd = open(device, O_RDWR | O_NONBLOCK, 0);
    if (fd == -1) quit("open");
    camera_t* camera = (camera_t*)malloc(sizeof (camera_t));
    camera->fd = fd;
    camera->width = width;
    camera->height = height;
    camera->buffer_count = 0;
    camera->buffers = NULL;
    camera->head.length = 0;
    camera->head.start = NULL;
    return camera;
}

void CCamera::CCamera_init(camera_t* camera)
{
    struct v4l2_capability cap;
    if (xioctl(camera->fd, VIDIOC_QUERYCAP, &cap) == -1) quit("VIDIOC_QUERYCAP");
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) quit("no capture");
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) quit("no streaming");

    struct v4l2_cropcap cropcap;
    memset(&cropcap, 0, sizeof cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(camera->fd, VIDIOC_CROPCAP, &cropcap) == 0) {
        struct v4l2_crop crop;
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;
        if (xioctl(camera->fd, VIDIOC_S_CROP, &crop) == -1) {
        // cropping not supported
        }
    }

    struct v4l2_format format;
    memset(&format, 0, sizeof format);
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = camera->width;
    format.fmt.pix.height = camera->height;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(camera->fd, VIDIOC_S_FMT, &format) == -1) quit("VIDIOC_S_FMT");

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof req);
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(camera->fd, VIDIOC_REQBUFS, &req) == -1) quit("VIDIOC_REQBUFS");
    camera->buffer_count = req.count;
    camera->buffers = (buffer_t*)calloc(req.count, sizeof (buffer_t));

    size_t buf_max = 0;
    for (size_t i = 0; i < camera->buffer_count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(camera->fd, VIDIOC_QUERYBUF, &buf) == -1)
            quit("VIDIOC_QUERYBUF");
        if (buf.length > buf_max) buf_max = buf.length;
        camera->buffers[i].length = buf.length;
        camera->buffers[i].start = (uint8_t *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,camera->fd, buf.m.offset);
        if (camera->buffers[i].start == MAP_FAILED) quit("mmap");
    }
    camera->head.start = (uint8_t*)malloc(buf_max);
}

void CCamera::CCamera_start(camera_t* camera)
{
    for (size_t i = 0; i < camera->buffer_count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1) quit("VIDIOC_QBUF");
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(camera->fd, VIDIOC_STREAMON, &type) == -1) quit("VIDIOC_STREAMON");
}

void CCamera::CCamera_stop(camera_t* camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(camera->fd, VIDIOC_STREAMOFF, &type) == -1) quit("VIDIOC_STREAMOFF");
}

void CCamera::CCamera_finish(camera_t* camera)
{
    for (size_t i = 0; i < camera->buffer_count; i++)
    {
        munmap(camera->buffers[i].start, camera->buffers[i].length);
    }
    free(camera->buffers);
    camera->buffer_count = 0;
    camera->buffers = NULL;
    free(camera->head.start);
    camera->head.length = 0;
    camera->head.start = NULL;
}

void CCamera::CCamera_close(camera_t* camera)
{
    if(camera == NULL) 
	    return;
    CCamera_stop(camera);
    CCamera_finish(camera);
    if (close(camera->fd) == -1) 
	    printf("close camera\n");//quit("close");
    free(camera);
}

int CCamera::CCamera_capture(camera_t* camera)
{
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(camera->fd, VIDIOC_DQBUF, &buf) == -1) return -1;
    memcpy(camera->head.start, camera->buffers[buf.index].start, buf.bytesused);
    camera->head.length = buf.bytesused;
    if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1) return -1;
    return 0;
}

uint8_t* CCamera::yuyv2rgb(uint8_t* yuyv, uint32_t width, uint32_t height)
{
    uint8_t* rgb = (uint8_t*)calloc(width * height * 3, sizeof (uint8_t));
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j += 2) {
        size_t index = i * width + j;
        int y0 = yuyv[index * 2 + 0] << 8;
        int u = yuyv[index * 2 + 1] - 128;
        int y1 = yuyv[index * 2 + 2] << 8;
        int v = yuyv[index * 2 + 3] - 128;
        rgb[index * 3 + 0] = minmax(0, (y0 + 359 * v) >> 8, 255);
        rgb[index * 3 + 1] = minmax(0, (y0 + 88 * v - 183 * u) >> 8, 255);
        rgb[index * 3 + 2] = minmax(0, (y0 + 454 * u) >> 8, 255);
        rgb[index * 3 + 3] = minmax(0, (y1 + 359 * v) >> 8, 255);
        rgb[index * 3 + 4] = minmax(0, (y1 + 88 * v - 183 * u) >> 8, 255);
        rgb[index * 3 + 5] = minmax(0, (y1 + 454 * u) >> 8, 255);
        }
    }
    return rgb;

}

int CCamera::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = 220*r/256;
    pixel[1] = 220*g/256;
    pixel[2] = 220*b/256;
    return pixel32;
}
int CCamera::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
     unsigned int in, out = 0;
     unsigned int pixel_16;
     unsigned char pixel_24[3];
     unsigned int pixel32;
     int y0, u, y1, v;
     for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 =
            yuv[in + 3] << 24 |
            yuv[in + 2] << 16 |
            yuv[in + 1] <<  8 |
            yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
     }
     return 0;
}


void CCamera::rgb2jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality)
{
    JSAMPARRAY image;
    image = (JSAMPARRAY)calloc(height, sizeof (JSAMPROW));
    for (size_t i = 0; i < height; i++) {
        image[i] = (JSAMPROW)calloc(width * 3, sizeof (JSAMPLE));
        for (size_t j = 0; j < width; j++) {
            image[i][j * 3 + 0] = rgb[(i * width + j) * 3 + 0];
            image[i][j * 3 + 1] = rgb[(i * width + j) * 3 + 1];
            image[i][j * 3 + 2] = rgb[(i * width + j) * 3 + 2];
        }
    }

    struct jpeg_compress_struct compress;
    struct jpeg_error_mgr error;
    compress.err = jpeg_std_error(&error);
    jpeg_create_compress(&compress);
    jpeg_stdio_dest(&compress, dest);

    compress.image_width = width;
    compress.image_height = height;
    compress.input_components = 3;
    compress.in_color_space = JCS_RGB;
    jpeg_set_defaults(&compress);
    jpeg_set_quality(&compress, quality, TRUE);
    jpeg_start_compress(&compress, TRUE);
    jpeg_write_scanlines(&compress, image, height);
    jpeg_finish_compress(&compress);
    jpeg_destroy_compress(&compress);

    for (size_t i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);

}


camera_t* CCamera::CCamera_OPEN(const char * device,uint32_t width, uint32_t height)
{
    if (access(device, F_OK) == -1) quit("device is not exist");
    camera_t* camera = CCamera_open(device, width, height);
    CCamera_init(camera);
    CCamera_start(camera);
    return camera;
}

void CCamera::CRgb2Mat(unsigned char *src,Mat &dst,unsigned int width,unsigned int height,unsigned int channels )
{
    dst=Mat(height,width,CV_8UC3);
    memcpy(dst.data,src,width*height*channels);
}













