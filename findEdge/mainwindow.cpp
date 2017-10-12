#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    namedWindow("原图",WINDOW_NORMAL);
    namedWindow("灰度图",WINDOW_NORMAL);
    namedWindow("阈值化",WINDOW_NORMAL);
    namedWindow("Canny边缘检测",WINDOW_NORMAL);
    namedWindow("轮廓图",WINDOW_NORMAL);
    namedWindow("最小包围矩形图",WINDOW_NORMAL);

    //【1】图片预处理，转化为灰度图片并阈值化突出边缘轮廓
    srcMat = imread("3.jpg");
    if(!srcMat.data)
        QMessageBox::warning(this,"warning","can not load image");
    imshow("原图",srcMat);
    cvtColor(srcMat,grayMat,CV_BGR2GRAY);
    imshow("灰度图",grayMat);
    connect(ui->pushButton_2,&QPushButton::clicked,[this]{
        threshold(grayMat,threMat,ui->lineEdit_2->text().toInt(),255,CV_THRESH_BINARY_INV);
        imshow("阈值化",threMat);
    });

    //【2】利用canny算子的高低阈值检测边缘
    connect(ui->horizontalSlider_2,&QSlider::valueChanged,this,&MainWindow::showImg);
    connect(ui->horizontalSlider_3,&QSlider::valueChanged,this,&MainWindow::showImg);

    //【3】寻找边缘轮廓，并进行校验检测边界
    connect(ui->pushButton_3,&QPushButton::clicked,[this]{
        //绘制轮廓图
        vector<Vec4i>hierarchy(10000);
        vector<Mat>contours(10000);//手动分配内存空间大小
        findContours(cannyMat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        Mat outlineMat(cannyMat.size(),CV_8U,Scalar(255));
        drawContours(outlineMat,contours,-1,Scalar(0),3);
        imshow("轮廓图",outlineMat);
        vector<RotatedRect> rects;
        //Mat edgeMat(cannyMat.size(),CV_8U,Scalar(255));
        for(vector<Mat>::iterator itc = contours.begin();itc!=contours.end();itc++)
        {
            //包围轮廓的最小矩形
            RotatedRect mr = minAreaRect(Mat(*itc));
            if(verifySizes(mr))
            {
                Point2f vertex[4];
                mr.points(vertex);
                for(int i =0;i<4;i++)
                    line(srcMat,vertex[i],vertex[(i+1)%4],Scalar(0,0,255),2,CV_AA);
                qDebug() << QString::fromLocal8Bit("顶点坐标1") << vertex[0].x << vertex[0].y;
                qDebug() << QString::fromLocal8Bit("顶点坐标2") << vertex[1].x << vertex[1].y;
                qDebug() << QString::fromLocal8Bit("顶点坐标3") << vertex[2].x << vertex[2].y;
                qDebug() << QString::fromLocal8Bit("顶点坐标4") << vertex[3].x << vertex[3].y;
                rects.push_back(mr);
                qDebug() << QString::fromLocal8Bit("符合标准的矩形数量：") << rects.size();
            }
        }
        imshow("最小包围矩形图",srcMat);
    });


    //    connect(ui->pushButton,&QPushButton::clicked,[this]{
    //        GaussianBlur(threMat,blurMat,Size(3,3),ui->lineEdit->text().toDouble());
    //        imshow("高斯滤波",blurMat);
    //    });
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
    const float aspect = 1.52;
    int min = 400*aspect*400;
    int max = 900*aspect*900;
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
