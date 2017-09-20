#include "dialog.h"
#include "ui_dialog.h"

QImage Dialog::Mat2Img(Mat &mat)
{
    //cvtColor 接受const类型的引用输入，然后在内部创造一个局部变量获得其数据，并创造一个局部变量，然后做处理，修改_dst输入
    cvtColor(mat,mat,CV_BGR2RGB);

     return QImage(static_cast<const unsigned char*>(mat.data),
                   mat.cols,mat.rows,
                   mat.cols*mat.channels(),
                   QImage::Format_RGB888);

}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),m_condition(true)
{
    ui->setupUi(this);
    //*************************将Mat变为label*************************//
//    Mat m;
//    m = imread("1.jpg");
//    QImage image = Mat2Img(m);
//    QPixmap pixmap = QPixmap::fromImage(image);
//    ui->label->setPixmap(pixmap);
//    ui->label->resize(pixmap.size());
    Mat m;
    m = imread("1.jpg");
    qDebug() << &m;
    imshow("原图",m);
    cvtColor(m,m,CV_BGR2GRAY);
    qDebug() << &m;
    imshow("处理图",m);
}

Dialog::~Dialog()
{
    delete ui;
}



