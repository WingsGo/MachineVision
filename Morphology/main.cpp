#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <opencv2/opencv.hpp>
using namespace cv;

Mat g_srcImage;
Mat g_dstImage;
int g_nElementShape = MORPH_RECT;
int g_nMaxValue = 10;
int g_nOpenCloseNum = 0;
int g_nDilateErodeNum = 0;
int g_nTopBlackHatNum =0;

static void on_OpenClose(int,void *);
static void on_DilateErode(int,void *);
static void on_TopBlackHatNum(int,void *);

//**********************************erode 和 dilate是针对白色区域，而开运算和闭运算可以联想开门与关门

int main(int argc, char *argv[])
{
    g_srcImage = imread("../1.jpg");
    if(!g_srcImage.data) qDebug() << QString::fromLocal8Bit("图片读取失败");
    namedWindow("【原图】",0);
    imshow("【原图】",g_srcImage);
    namedWindow("【开/闭运算】",0);
    namedWindow("【膨胀/腐蚀】",0);
    namedWindow("【顶帽/黑帽】",0);
    createTrackbar("内核","【开/闭运算】",&g_nOpenCloseNum,g_nMaxValue*2+1,on_OpenClose);
    createTrackbar("内核","【膨胀/腐蚀】",&g_nDilateErodeNum,g_nMaxValue*2+1,on_DilateErode);
    createTrackbar("内核","【顶帽/黑帽】",&g_nTopBlackHatNum,g_nMaxValue*2+1,on_TopBlackHatNum);

    while (true)
    {
        int c;
        on_OpenClose(g_nOpenCloseNum,0);
        on_DilateErode(g_nDilateErodeNum,0);
        on_TopBlackHatNum(g_nTopBlackHatNum,0);
        c = waitKey(0);
        if((char)c == 'q' || (char)c == 'Esc')
            break;
        else if(c == 1)
            g_nElementShape = MORPH_RECT;
        else if(c == 2)
            g_nElementShape = MORPH_ELLIPSE;
        else if(c == 3)
            g_nElementShape = MORPH_CROSS;
        else if(c == 4)
            g_nElementShape = (g_nElementShape+1)%3;
    }
    return 0;
}

static void on_OpenClose(int,void *)
{
    int offset_value = g_nOpenCloseNum - g_nMaxValue*2+1;
    int abs_offset_value = offset_value>0?offset_value:-offset_value;
    Mat element = getStructuringElement(g_nElementShape,Size(abs_offset_value*2+1,abs_offset_value*2+1),Point(abs_offset_value,abs_offset_value));
    if(offset_value>0)
    {
        morphologyEx(g_srcImage,g_dstImage,MORPH_OPEN,element);
    }
    else
    {
        morphologyEx(g_srcImage,g_dstImage,MORPH_CLOSE,element);
    }
    imshow("【开/闭运算】",g_dstImage);
}

static void on_DilateErode(int,void *)
{
    int offset_value = g_nDilateErodeNum - g_nMaxValue*2+1;
    int abs_offset_value = offset_value>0?offset_value:-offset_value;
    Mat element = getStructuringElement(g_nElementShape,Size(abs_offset_value*2+1,abs_offset_value*2+1),Point(abs_offset_value,abs_offset_value));
    if(offset_value>0)
        dilate(g_srcImage,g_dstImage,element);
    else
        erode(g_srcImage,g_dstImage,element);
    imshow("【膨胀/腐蚀】",g_dstImage);
}

static void on_TopBlackHatNum(int,void *)
{
    int offset_value = g_nTopBlackHatNum - g_nMaxValue*2+1;
    int abs_offset_value = offset_value>0?offset_value:-offset_value;
    Mat element = getStructuringElement(g_nElementShape,Size(abs_offset_value*2+1,abs_offset_value*2+1),Point(abs_offset_value,abs_offset_value));
    if(offset_value>0)
    {
        morphologyEx(g_srcImage,g_dstImage,MORPH_TOPHAT,element);
    }
    else
    {
        morphologyEx(g_srcImage,g_dstImage,MORPH_BLACKHAT,element);
    }
    imshow("【顶帽/黑帽】",g_dstImage);
}
