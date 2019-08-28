#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QImage>
#include <QDebug>
#include <math.h>
#include <iostream>
#include "ui_mainwindow.h"
#include "video.h"
#include <sys/time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "include/pic_thread.h"
#include "net.h"
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;
using namespace cv;
const float nms_threshold_[3] = {0.5, 0.7, 0.7};
const float threshold_[3] = {0.8, 0.8, 0.8};
const float mean_vals[3] = {127.5, 127.5, 127.5};
const float norm_vals[3] = {0.0078125, 0.0078125, 0.0078125};

struct Bbox
{
	float score;
	int x1;
	int y1;
	int x2;
	int y2;
	float area;
	bool exist;
	float ppoint[10];
	float regreCoord[4];
};

struct orderScore
{
	float score;
	int oriOrder;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //int load_face_file(string file_path);
    //void detect_face(Mat &frame); 
    void detect(ncnn::Mat& img_, std::vector<Bbox>& finalBbox);
private slots:
    void pushButton_2_clicked();
    void getFrame();
    void pushButton_3_clicked();
    void pushButton_clicked();
    void on_pushButton_4_clicked();
private:
    Ui::MainWindow *ui;
    VideoCapture capture;
    QTimer *timer;
    PicThread *pic_thread;
    camera_t* camera = NULL;
    uint32_t width;
    uint32_t height;
    CCamera *C_camera = NULL;
    int rgb24Size;
    int rgb24Pitch;
    unsigned char *rgb24;
    int timer_count;
    string queue_data;
    bool timer_flag = false;
    bool flag=true;
    /*CascadeClassifier face_cascade;
    String face_cascade_name = "./haarcascade_frontalface_alt_tree.xml";*/
    void generateBbox(ncnn::Mat score, ncnn::Mat location, vector<Bbox>& boundingBox_, vector<orderScore>& bboxScore_, float scale);
    void nms(vector<Bbox> &boundingBox_, std::vector<orderScore> &bboxScore_, const float overlap_threshold_, string modelname="Union");
    void refineAndSquareBbox(vector<Bbox> &vecBbox, const int &height, const int &width);
    ncnn::Net Pnet, Rnet, Onet;
    ncnn::Mat img;

    std::vector<Bbox> firstBbox_, secondBbox_,thirdBbox_;
    std::vector<orderScore> firstOrderScore_, secondBboxScore_, thirdBboxScore_;
    int img_w, img_h;
};

#endif // MAINWINDOW_H
