#include "include/mainwindow.h"
#include "include/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Pnet.load_param("det1.param");
    Pnet.load_model("det1.bin");
    Rnet.load_param("det2.param");
    Rnet.load_model("det2.bin");
    Onet.load_param("det3.param");
    Onet.load_model("det3.bin");
    timer = new QTimer(this);
    width = 320;
    height = 240;
    rgb24Size = width * height * 3;
    rgb24Pitch = width * 3;
    rgb24 = new unsigned char[rgb24Size]();
    /*信号和槽*/
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(pushButton_2_clicked()),Qt::UniqueConnection);//打开摄像头按
    connect(timer, SIGNAL(timeout()), this, SLOT(getFrame()));//超时就读取当前摄像头信息
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(pushButton_3_clicked()));//拍照按钮
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(pushButton_clicked()));

}

MainWindow::~MainWindow()
{
	    delete ui;
}

bool cmpScore(orderScore lsh, orderScore rsh){
	if(lsh.score<rsh.score)
		return true;
	else
		return false;
}
static float getElapse(struct timeval *tv1,struct timeval *tv2)
{
	float t = 0.0f;
	if (tv1->tv_sec == tv2->tv_sec)
		t = (tv2->tv_usec - tv1->tv_usec)/1000.0f;
	else
		t = ((tv2->tv_sec - tv1->tv_sec) * 1000 * 1000 + tv2->tv_usec - tv1->tv_usec)/1000.0f;
	return t;
}

void MainWindow::pushButton_2_clicked()
{
	camera = C_camera->CCamera_open("/dev/video7", width, height);
	printf("width:%d,height:%d\n",camera->width,camera->height);
	C_camera->CCamera_init(camera);
	C_camera->CCamera_start(camera);
	timer->start(30);

}

void MainWindow::getFrame()
{
	Mat dst;
	C_camera->CCamera_capture(camera);
	C_camera->convert_yuv_to_rgb_buffer((unsigned char*)camera->head.start,(unsigned char*)rgb24, width, height);
	C_camera->CRgb2Mat(rgb24,dst,width,height,3);
	std::vector<Bbox> finalBbox;
	ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(dst.data, ncnn::Mat::PIXEL_BGR2RGB, dst.cols, dst.rows);
	struct timeval  tv1,tv2;
	struct timezone tz1,tz2;
	gettimeofday(&tv1,&tz1);
	detect(ncnn_img, finalBbox);
	gettimeofday(&tv2,&tz2);
	/*printf( "%s = %g ms \n ", "Detection All time", getElapse(&tv1, &tv2) );*/
	for(vector<Bbox>::iterator it=finalBbox.begin(); it!=finalBbox.end();it++){
		if((*it).exist){
			cv::rectangle(dst, Point((*it).x1, (*it).y1), Point((*it).x2, (*it).y2), Scalar(0,0,255), 1,8,0);
			/*for(int num=0;num<5;num++)circle(dst,Point((int)*(it->ppoint+num), (int)*(it->ppoint+num+5)),3,Scalar(0,255,255), -1);*/
		}
	}

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

void MainWindow::generateBbox(ncnn::Mat score, ncnn::Mat location, std::vector<Bbox>& boundingBox_, std::vector<orderScore>& bboxScore_, float scale){
	int stride = 2;
    	int cellsize = 12;
    	int count = 0;
    	//score p
    	float *p = score.channel(1);//score.data + score.cstep;
    	float *plocal = location.data;
    	Bbox bbox;
    	orderScore order;
    	for(int row=0;row<score.h;row++){
        	for(int col=0;col<score.w;col++){
            		if(*p>threshold_[0]){
                	bbox.score = *p;
                	order.score = *p;
                	order.oriOrder = count;
                	bbox.x1 = round((stride*col+1)/scale);
                	bbox.y1 = round((stride*row+1)/scale);
                	bbox.x2 = round((stride*col+1+cellsize)/scale);
                	bbox.y2 = round((stride*row+1+cellsize)/scale);
                	bbox.exist = true;
                	bbox.area = (bbox.x2 - bbox.x1)*(bbox.y2 - bbox.y1);
                	for(int channel=0;channel<4;channel++)
                    		bbox.regreCoord[channel]=location.channel(channel)[0];
                	boundingBox_.push_back(bbox);
                	bboxScore_.push_back(order);
                	count++;
            	}
            	p++;
            	plocal++;
        	}
    	}
}

void MainWindow::nms(std::vector<Bbox> &boundingBox_, std::vector<orderScore> &bboxScore_, const float overlap_threshold_, string modelname){
    if(boundingBox_.empty()){
        return;
    }
    std::vector<int> heros;
    //sort the score
    sort(bboxScore_.begin(), bboxScore_.end(), cmpScore);

    int order = 0;
    float IOU = 0;
    float maxX = 0;
    float maxY = 0;
    float minX = 0;
    float minY = 0;
    while(bboxScore_.size()>0){
        order = bboxScore_.back().oriOrder;
        bboxScore_.pop_back();
        if(order<0)continue;
        if(boundingBox_.at(order).exist == false) continue;
        heros.push_back(order);
        boundingBox_.at(order).exist = false;//delete it

        for(int num=0;num<boundingBox_.size();num++){
            if(boundingBox_.at(num).exist){
                //the iou
                maxX = (boundingBox_.at(num).x1>boundingBox_.at(order).x1)?boundingBox_.at(num).x1:boundingBox_.at(order).x1;
                maxY = (boundingBox_.at(num).y1>boundingBox_.at(order).y1)?boundingBox_.at(num).y1:boundingBox_.at(order).y1;
                minX = (boundingBox_.at(num).x2<boundingBox_.at(order).x2)?boundingBox_.at(num).x2:boundingBox_.at(order).x2;
                minY = (boundingBox_.at(num).y2<boundingBox_.at(order).y2)?boundingBox_.at(num).y2:boundingBox_.at(order).y2;
                //maxX1 and maxY1 reuse
                maxX = ((minX-maxX+1)>0)?(minX-maxX+1):0;
                maxY = ((minY-maxY+1)>0)?(minY-maxY+1):0;
                //IOU reuse for the area of two bbox
                IOU = maxX * maxY;
                if(!modelname.compare("Union"))
                    IOU = IOU/(boundingBox_.at(num).area + boundingBox_.at(order).area - IOU);
                else if(!modelname.compare("Min")){
                    IOU = IOU/((boundingBox_.at(num).area<boundingBox_.at(order).area)?boundingBox_.at(num).area:boundingBox_.at(order).area);
                }
                if(IOU>overlap_threshold_){
                    boundingBox_.at(num).exist=false;
                    for(vector<orderScore>::iterator it=bboxScore_.begin(); it!=bboxScore_.end();it++){
                        if((*it).oriOrder == num) {
                            (*it).oriOrder = -1;
                            break;
                        }
                    }
                }
            }
        }
    }
    for(int i=0;i<heros.size();i++)
        boundingBox_.at(heros.at(i)).exist = true;
}


void MainWindow::refineAndSquareBbox(vector<Bbox> &vecBbox, const int &height, const int &width){
    if(vecBbox.empty()){
        cout<<"Bbox is empty!!"<<endl;
        return;
    }
    float bbw=0, bbh=0, maxSide=0;
    float h = 0, w = 0;
    float x1=0, y1=0, x2=0, y2=0;
    for(vector<Bbox>::iterator it=vecBbox.begin(); it!=vecBbox.end();it++){
        if((*it).exist){
            bbw = (*it).x2 - (*it).x1 + 1;
            bbh = (*it).y2 - (*it).y1 + 1;
            x1 = (*it).x1 + (*it).regreCoord[0]*bbw;
            y1 = (*it).y1 + (*it).regreCoord[1]*bbh;
            x2 = (*it).x2 + (*it).regreCoord[2]*bbw;
            y2 = (*it).y2 + (*it).regreCoord[3]*bbh;

            w = x2 - x1 + 1;
            h = y2 - y1 + 1;

            maxSide = (h>w)?h:w;
            x1 = x1 + w*0.5 - maxSide*0.5;
            y1 = y1 + h*0.5 - maxSide*0.5;
            (*it).x2 = round(x1 + maxSide - 1);
            (*it).y2 = round(y1 + maxSide - 1);
            (*it).x1 = round(x1);
            (*it).y1 = round(y1);

            //boundary check
            if((*it).x1<0)(*it).x1=0;
            if((*it).y1<0)(*it).y1=0;
            if((*it).x2>width)(*it).x2 = width - 1;
            if((*it).y2>height)(*it).y2 = height - 1;

            it->area = (it->x2 - it->x1)*(it->y2 - it->y1);
        }
    }
}

void MainWindow::detect(ncnn::Mat& img_, std::vector<Bbox>& finalBbox_){
    firstBbox_.clear();
    firstOrderScore_.clear();
    secondBbox_.clear();
    secondBboxScore_.clear();
    thirdBbox_.clear();
    thirdBboxScore_.clear();

    img = img_;
    img_w = img.w;
    img_h = img.h;
    img.substract_mean_normalize(mean_vals, norm_vals);

    float minl = img_w<img_h?img_w:img_h;
    int MIN_DET_SIZE = 12;
    int minsize = 90;
    float m = (float)MIN_DET_SIZE/minsize;
    minl *= m;
    float factor = 0.709;
    int factor_count = 0;
    vector<float> scales_;
    while(minl>MIN_DET_SIZE){
        if(factor_count>0)m = m*factor;
        scales_.push_back(m);
        minl *= factor;
        factor_count++;
    }
    orderScore order;
    int count = 0;
    for (size_t i = 0; i < scales_.size(); i++) {
           int hs = (int)ceil(img_h*scales_[i]);
           int ws = (int)ceil(img_w*scales_[i]);
           //ncnn::Mat in = ncnn::Mat::from_pixels_resize(image_data, ncnn::Mat::PIXEL_RGB2BGR, img_w, img_h, ws, hs);
           ncnn::Mat in;
           resize_bilinear(img_, in, ws, hs);
           //in.substract_mean_normalize(mean_vals, norm_vals);
           ncnn::Extractor ex = Pnet.create_extractor();
           ex.set_light_mode(true);
           ex.input("data", in);
           ncnn::Mat score_, location_;
           ex.extract("prob1", score_);
           ex.extract("conv4-2", location_);
           std::vector<Bbox> boundingBox_;
           std::vector<orderScore> bboxScore_;
           generateBbox(score_, location_, boundingBox_, bboxScore_, scales_[i]);
           nms(boundingBox_, bboxScore_, nms_threshold_[0]);

           for(vector<Bbox>::iterator it=boundingBox_.begin(); it!=boundingBox_.end();it++){
               if((*it).exist){
                   firstBbox_.push_back(*it);
                   order.score = (*it).score;
                   order.oriOrder = count;
                   firstOrderScore_.push_back(order);
                   count++;
               }
           }
           bboxScore_.clear();
           boundingBox_.clear();
       }
       //the first stage's nms
       if(count<1)return;
       nms(firstBbox_, firstOrderScore_, nms_threshold_[0]);
       refineAndSquareBbox(firstBbox_, img_h, img_w);
       //printf("firstBbox_.size()=%d\n", firstBbox_.size());
       //second stage
           count = 0;
           for(vector<Bbox>::iterator it=firstBbox_.begin(); it!=firstBbox_.end();it++){
               if((*it).exist){
                   ncnn::Mat tempIm;
                   copy_cut_border(img, tempIm, (*it).y1, img_h-(*it).y2, (*it).x1, img_w-(*it).x2);
                   ncnn::Mat in;
                   resize_bilinear(tempIm, in, 24, 24);
                   ncnn::Extractor ex = Rnet.create_extractor();
                   ex.set_light_mode(true);
                   ex.input("data", in);
                   ncnn::Mat score, bbox;
                   ex.extract("prob1", score);
                   ex.extract("conv5-2", bbox);
                   if(*(score.data+score.cstep)>threshold_[1]){
                       for(int channel=0;channel<4;channel++)
                           it->regreCoord[channel]=bbox.channel(channel)[0];//*(bbox.data+channel*bbox.cstep);
                       it->area = (it->x2 - it->x1)*(it->y2 - it->y1);
                       it->score = score.channel(1)[0];//*(score.data+score.cstep);
                       secondBbox_.push_back(*it);
                       order.score = it->score;
                       order.oriOrder = count++;
                       secondBboxScore_.push_back(order);
                   }
                   else{
                       (*it).exist=false;
                   }
               }
           }
          // printf("secondBbox_.size()=%d\n", secondBbox_.size());
           if(count<1)return;
           nms(secondBbox_, secondBboxScore_, nms_threshold_[1]);
           refineAndSquareBbox(secondBbox_, img_h, img_w);
           //third stage
              count = 0;
              for(vector<Bbox>::iterator it=secondBbox_.begin(); it!=secondBbox_.end();it++){
                  if((*it).exist){
                      ncnn::Mat tempIm;
                      copy_cut_border(img, tempIm, (*it).y1, img_h-(*it).y2, (*it).x1, img_w-(*it).x2);
                      ncnn::Mat in;
                      resize_bilinear(tempIm, in, 48, 48);
                      ncnn::Extractor ex = Onet.create_extractor();
                      ex.set_light_mode(true);
                      ex.input("data", in);
                      ncnn::Mat score, bbox, keyPoint;
                      ex.extract("prob1", score);
                      ex.extract("conv6-2", bbox);
                      ex.extract("conv6-3", keyPoint);
                      if(score.channel(1)[0]>threshold_[2]){
                          for(int channel=0;channel<4;channel++)
                              it->regreCoord[channel]=bbox.channel(channel)[0];
                          it->area = (it->x2 - it->x1)*(it->y2 - it->y1);
                          it->score = score.channel(1)[0];
                          for(int num=0;num<5;num++){
                              (it->ppoint)[num] = it->x1 + (it->x2 - it->x1)*keyPoint.channel(num)[0];
                              (it->ppoint)[num+5] = it->y1 + (it->y2 - it->y1)*keyPoint.channel(num+5)[0];
                          }

                          thirdBbox_.push_back(*it);
                          order.score = it->score;
                          order.oriOrder = count++;
                          thirdBboxScore_.push_back(order);
                      }
                      else
                          (*it).exist=false;
                      }
                  }

             // printf("thirdBbox_.size()=%d\n", thirdBbox_.size());
              if(count<1)return;
              refineAndSquareBbox(thirdBbox_, img_h, img_w);
              nms(thirdBbox_, thirdBboxScore_, nms_threshold_[2], "Min");
              finalBbox_ = thirdBbox_;
          }



