#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QMessageBox>
#include <QDebug>

#include <opencv2/opencv.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QImage Mat2QImage(const Mat &mat);
    bool verifySizes(RotatedRect candidate);

private slots:
    void showImg();

private:
    Ui::MainWindow *ui;
    Mat srcMat;
    Mat grayMat,blurMat,cannyMat,threMat;
};

#endif // MAINWINDOW_H
