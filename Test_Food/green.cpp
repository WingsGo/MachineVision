#include <opencv2/opencv.hpp>
#include <QMessageBox>
using namespace cv;


void on_thresholdChange(int,void*);

int g_threshold = 50;
Mat src,threImg,dst,gray,blurImg;

int main(int argc, char *argv[])
{
    src = imread("../装盘青菜.jpg");
    if(!src.data)
    {
        QMessageBox::warning(nullptr,"warning","load image failed");
        return false;
    }
    //namedWindow("线变换",WINDOW_NORMAL);
    namedWindow("直线",WINDOW_NORMAL);
    namedWindow("原图",WINDOW_NORMAL);
    imshow("原图",src);

    //阈值化处理强化边缘
    threshold(src,threImg,200,256,CV_THRESH_BINARY);
    //imshow("阈值化",threImg);

    //得到灰度图
    cvtColor(threImg,gray,CV_BGR2GRAY);
    //imshow("灰度图",gray);

    //高斯滤波
    GaussianBlur(gray,blurImg,Size(3,3),0.618);
    //imshow("滤波图",blurImg);

    //canny算子
    Canny(gray,dst,3,9);
    //imshow("Canny",dst);

    //sobel算子
    Mat grad_x,grad_y;
    Mat abs_grad_x,abs_grad_y;
    Sobel(gray,grad_x,CV_16S,1,0,3,1,1,BORDER_DEFAULT);
    convertScaleAbs(grad_x,abs_grad_x);
    Sobel(gray,grad_y,CV_16S,1,0,3,1,1,BORDER_DEFAULT);
    convertScaleAbs(grad_y,abs_grad_y);
    //imshow("xSobel",abs_grad_x);
    //imshow("ySobel",abs_grad_y);

    //霍夫线变换
    vector<Vec2f> lines;
    HoughLines(dst,lines,1,CV_PI/180,210,0,0);
    Mat lineImg(dst.size(),dst.type(),Scalar::all(255));

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( dst, pt1, pt2, Scalar(55,100,195), 1, CV_AA);
        line( lineImg, pt1, pt2, Scalar(0,0,0), 1, CV_AA);
    }

    //imshow("线变换",dst);
    imshow("直线",lineImg);

    int quit = waitKey();
    if((char)quit == 'q')
        return 0;
}

void on_thresholdChange(int,void*)
{
    threshold(src,threImg,g_threshold,256,CV_THRESH_BINARY);
    imshow("阈值化",threImg);
}


