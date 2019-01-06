#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    width = 160;
    height = 120;

    rgb24Size = width * height * 3;
    rgb24Pitch = width * 3;
    rgb24 = new unsigned char[rgb24Size]();
    /*信号和槽*/
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(pushButton_2_clicked()),Qt::UniqueConnection);//打开摄像头按钮
    connect(timer, SIGNAL(timeout()), this, SLOT(getFrame()));//超时就读取当前摄像头信息
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(pushButton_3_clicked()));//拍照按钮
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(pushButton_clicked()));
    //if the function name is on_pushButton_clicked no need write connect,like on_pushButton_4_clicked;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButton_2_clicked()
{
    camera = C_camera->CCamera_open("/dev/video7", width, height);
    printf("width:%d,height:%d\n",camera->width,camera->height);
    C_camera->CCamera_init(camera);
    C_camera->CCamera_start(camera);
    load_face_file(face_cascade_name);
    timer->start(30);

}

void MainWindow::getFrame()
{
    Mat dst;
    C_camera->CCamera_capture(camera);
    C_camera->convert_yuv_to_rgb_buffer((unsigned char*)camera->head.start,(unsigned char*)rgb24, width, height);
    C_camera->CRgb2Mat(rgb24,dst,width,height,3);
    detect_face(dst);
    const unsigned char *IMG = (const unsigned char *)dst.data;
    QImage *mImage = new QImage(IMG, width, height, rgb24Pitch, QImage::Format_RGB888);
    *mImage = mImage->scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label->setPixmap(QPixmap::fromImage(*mImage));
}

void MainWindow::pushButton_3_clicked()
{
    Mat dst;
    C_camera->CCamera_capture(camera);
    C_camera->convert_yuv_to_rgb_buffer((unsigned char*)camera->head.start,(unsigned char*)rgb24, width, height);
    C_camera->CRgb2Mat(rgb24,dst,width,height,3);
    //detect_face(dst);
    const unsigned char *IMG = (const unsigned char *)dst.data;
    QImage *image = new QImage(IMG, width, height, rgb24Pitch, QImage::Format_RGB888);
    image->save("/usr/local/result.jpg");
    *image = image->scaled(ui->label_2->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_2->setPixmap(QPixmap::fromImage(*image));
}

void MainWindow::pushButton_clicked()
{
    timer->stop();
    C_camera->CCamera_close(camera);
    delete timer;
    delete[] rgb24;
    this->close();

}

void MainWindow::on_pushButton_4_clicked()
{
    flag = !flag;
    if (flag)
    {
        ui->pushButton_4->setText("Stop");
        timer->start();
        C_camera->CCamera_start(camera);
    }
    else
    {
        ui->pushButton_4->setText("Start");
        timer->stop();
        C_camera->CCamera_stop(camera);
    }

}

int MainWindow::load_face_file(string file_path)
{
    if( !face_cascade.load( file_path ) )
    {
        printf("--(!)Error loading face cascade\n");
        return -1;
    }
    return 0;
}

void MainWindow::detect_face(Mat &frame)
{
    vector<Rect> faces;
    Mat frame_gray;


    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );

    equalizeHist( frame_gray, frame_gray );
    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.2, 2, 0, Size(30, 30) );
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Point retP1,retP2;
        retP1.x = faces[i].x;
        retP1.y = faces[i].y;
        retP2.x = faces[i].x+faces[i].width;
        retP2.y = faces[i].y+faces[i].height;
        rectangle(frame, retP1, retP2, Scalar(0,255,0), 1, 4, 0);
        printf("here\n");
    }
}









