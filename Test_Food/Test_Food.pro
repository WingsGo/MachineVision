#-------------------------------------------------
#
# Project created by QtCreator 2017-09-22T11:05:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test_Food
TEMPLATE = app


SOURCES += main.cpp \
    green.cpp

HEADERS  +=

FORMS    += mainwindow.ui

INCLUDEPATH+=d:/opencv/build/include/opencv \
                    d:/opencv/build/include/opencv2 \
                   d:/opencv/build/include

win32:CONFIG(debug, debug|release): {
LIBS += -LD:\opencv\build\x86\vc10\lib \
-lopencv_core249d \
-lopencv_imgproc249d \
-lopencv_highgui249d \
-lopencv_ml249d \
-lopencv_video249d \
-lopencv_features2d249d \
-lopencv_calib3d249d \
-lopencv_objdetect249d \
-lopencv_contrib249d \
-lopencv_legacy249d \
-lopencv_flann249d
} else:win32:CONFIG(release, debug|release): {
LIBS += -LD:\opencv\build\x86\vc10\lib \
-lopencv_core249 \
-lopencv_imgproc249 \
-lopencv_highgui249 \
-lopencv_ml249 \
-lopencv_video249 \
-lopencv_features2d249 \
-lopencv_calib3d249 \
-lopencv_objdetect249 \
-lopencv_contrib249 \
-lopencv_legacy249 \
-lopencv_flann249
}
