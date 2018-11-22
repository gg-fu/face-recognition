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
    width = 640;
    height = 480;

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
    timer->start(50);

}

void MainWindow::getFrame()
{
    C_camera->CCamera_capture(camera);
    C_camera->convert_yuv_to_rgb_buffer((unsigned char*)camera->head.start,(unsigned char*)rgb24, width, height);
    QImage *mImage = new QImage((unsigned char *)rgb24, width, height, rgb24Pitch, QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(*mImage));
}

void MainWindow::pushButton_3_clicked()
{
    C_camera->CCamera_capture(camera);
    C_camera->convert_yuv_to_rgb_buffer((unsigned char*)camera->head.start,(unsigned char*)rgb24, width, height);
    QImage *image = new QImage((unsigned char *)rgb24, width, height, rgb24Pitch, QImage::Format_RGB888);
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





