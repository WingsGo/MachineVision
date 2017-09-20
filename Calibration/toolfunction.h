// toolFunction.h
#ifndef TOOL_FUNCTION_H
#pragma once
#define TOOL_FUNCTION_H

#include<iostream>
#include <Windows.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <string>

#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

class CalibrationAssist
{
public:
    CalibrationAssist() {}
    ~CalibrationAssist() {}

public:
    std::vector<std::string>get_filelist( std::string foldname );

    cv::Point3d triangulate( std::vector<cv::Mat_<double>> &ProjectMats,
        std::vector<cv::Point2d> &imagePoints );

    void save3dPoint( std::string path_, std::vector<cv::Point3d> &Point3dLists );
};
#endif // TOOL_FUNCTION_H
