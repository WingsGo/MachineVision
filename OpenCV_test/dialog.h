#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDir>
#include <QDebug>
#include <QString>

#include <opencv2/opencv.hpp>
using namespace cv;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT


public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    QImage Mat2Img(Mat& mat);

private:
    Ui::Dialog *ui;
    bool m_condition;

};

#endif // DIALOG_H
