#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //图片预处理，灰度化并进行高斯滤波
    srcMat = imread("1.jpg");
    if(!srcMat.data)
        QMessageBox::warning(this,"warning","can not load image");
    imshow("原图",srcMat);
    cvtColor(srcMat,grayMat,CV_BGR2GRAY);
    imshow("灰度图",grayMat);
    connect(ui->pushButton_2,&QPushButton::clicked,[this]{
        threshold(grayMat,threMat,ui->lineEdit_2->text().toInt(),255,CV_THRESH_BINARY_INV);
        imshow("阈值化",threMat);
    });
    connect(ui->pushButton,&QPushButton::clicked,[this]{
        GaussianBlur(threMat,blurMat,Size(3,3),ui->lineEdit->text().toDouble());
        imshow("高斯滤波",blurMat);
    });
    connect(ui->horizontalSlider_2,&QSlider::valueChanged,this,&MainWindow::showImg);
    connect(ui->horizontalSlider_3,&QSlider::valueChanged,this,&MainWindow::showImg);
    connect(ui->pushButton_3,&QPushButton::clicked,[this]{
        //绘制轮廓图
        vector<Vec4i>hierarchy(10000);
        vector<Mat>contours(10000);//手动分配内存空间大小
        findContours(cannyMat, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
        Mat result(cannyMat.size(),CV_8U,Scalar(255));
        drawContours(result,contours,-1,Scalar(0),3);
        imshow("轮廓图",result);
        vector<RotatedRect> rects;
        Mat minRect(cannyMat.size(),CV_8U,Scalar(255));
        for(vector<Mat>::iterator itc = contours.begin();itc!=contours.end();itc++)
        {
            RotatedRect mr = minAreaRect(Mat(*itc));
            if(verifySizes(mr))
            {
                Point2f vertex[4];
                mr.points(vertex);
                for(int i =0;i<4;i++)
                    line(minRect,vertex[i],vertex[(i+1)%4],Scalar(0,0,0),2,CV_AA);
                rects.push_back(mr);
            }

//            if(!verifySizes(mr))
//                itc = contours.erase(itc);
//            else
        }
        imshow("最小包围矩形图",minRect);
    });
    connect(ui->pushButton_5,&QPushButton::clicked,[this]{
        vector<Vec4i> lines(1000);
        HoughLinesP(cannyMat,lines,1,CV_PI/180,80,50,10);
        qDebug() << lines.size();
        Mat houghMay(cannyMat.size(),CV_8U,Scalar(255));
        for(size_t i=0;i<lines.size();i++)
        {
            Vec4i lineo = lines[i];
            line(houghMay,Point(lineo[0],lineo[1]),Point(lineo[2],lineo[3]),Scalar(0,0,0),3,CV_AA);
        }
        imshow("霍夫变换",houghMay);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::Mat2QImage(const Mat &mat)
{
    return QImage((const unsigned char*)mat.data,mat.cols,mat.rows,mat.cols*mat.channels(),QImage::Format_RGB888);
}

bool MainWindow::verifySizes(RotatedRect candidate)
{
    //根据长宽比和面积来验证
    float error = 0.4;
    const float aspect = 2;
    int min = 355*355*aspect;
    int max = 1350*aspect*1350;
    float rmin = aspect-aspect*error;
    float rmax = aspect+aspect*error;

    int area = candidate.size.height * candidate.size.width;
    float r = (float)candidate.size.width/(float)candidate.size.height;
    if(r<1)
        r = 1/r;
    if((area < min || area > max) || (r<rmin || r>rmax))
        return false;
    else
        return true;
}

void MainWindow::showImg()
{
    Canny(threMat,cannyMat,ui->horizontalSlider_2->value(),ui->horizontalSlider_3->value());
    imshow("Canny边缘检测",cannyMat);
}
