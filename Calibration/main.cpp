/*
   Calibrate camera with chess board pattern.

   - Editor: Menghan Xia, Yahui Liu.
   - Data:   2015-07-28
   - Email:  yahui.cvrs@gmail.com
   - Address: Computer Vision and Remote Sensing(CVRS) Lab, Wuhan University.
**/

#include<iostream>
#include <vector>
#include <string>

#include "cv.h"
#include "highgui.h"

#include "toolFunction.h"
#define DEBUG_OUTPUT_INFO

using namespace std;
using namespace cv;

void main()
{
    char* folderPath = "E:/Images/New";           // image folder
    std::vector<std::string> graphPaths;
    std::vector<std::string> graphSuccess;

    CalibrationAssist calAssist;

    graphPaths = calAssist.get_filelist(folderPath); // collect image list

#ifdef DEBUG_OUTPUT_INFO
    std::cout << "loaded " << graphPaths.size() << " images"<< std::endl;
#endif

    if ( !graphPaths.empty() )
    {
#ifdef DEBUG_OUTPUT_INFO
        std::cout << "Start corner detection ..." << std::endl;
#endif

        cv::Mat curGraph;  // current image
        cv::Mat gray;      // gray image of current image

        int imageCount = graphPaths.size();
        int imageCountSuccess = 0;
        cv::Size image_size;
        cv::Size boardSize  = cv::Size(19, 19);     // chess board pattern size
        cv::Size squareSize = cv::Size(15, 15);     // grid physical size, as a scale factor

        std::vector<cv::Point2f> corners;                  // one image corner list
        std::vector<std::vector<cv::Point2f> > seqCorners; // n images corner list

        if ( graphPaths.size() < 3 )
        {
#ifdef DEBUG_OUTPUT_INFO
            std::cout << "Calibrate failed, with less than three images!" << std::endl;
#endif
            return ;
        }

        for ( int i=0; i<graphPaths.size(); i++ )
        {
            string graphpath = folderPath;
            graphpath += "/" + graphPaths[i];
            curGraph = cv::imread(graphpath);

            if ( curGraph.channels() == 3 )
                cv::cvtColor( curGraph, gray, CV_BGR2GRAY );
            else
                curGraph.copyTo( gray );

            // for every image, empty the corner list
            std::vector<cv::Point2f>().swap( corners );

            // corners detection
            bool success = cv::findChessboardCorners( curGraph, boardSize, corners );

            if ( success ) // succeed
            {
#ifdef DEBUG_OUTPUT_INFO
                std::cout << i << " " << graphPaths[i] << " succeed"<< std::endl;
#endif
                int row = curGraph.rows;
                int col = curGraph.cols;

                graphSuccess.push_back( graphpath );
                imageCountSuccess ++;

                image_size = cv::Size( col, row );

                // find sub-pixels
                cv::cornerSubPix(
                    gray,
                    corners,
                    cv::Size( 11, 11 ),
                    cv::Size( -1, -1 ),
                    cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ) );
                seqCorners.push_back( corners );

#if 1
                // draw corners and show them in current image
                cv::Mat imageDrawCorners;
                if ( curGraph.channels() == 3 )
                    curGraph.copyTo( imageDrawCorners );
                else
                    cv::cvtColor( curGraph, imageDrawCorners, CV_GRAY2RGB );

                for ( int j = 0; j < corners.size(); j ++)
                {
                    cv::Point2f dotPoint = corners[j];
                    cv::circle( imageDrawCorners, dotPoint, 3.0, cv::Scalar( 0, 255, 0 ), -1 );
                    cv::Point2f pt_m = dotPoint + cv::Point2f(4,4);
                    char text[100];
                    sprintf( text, "%d", j+1 );  // corner indexes which start from 1
                    cv::putText( imageDrawCorners, text, pt_m, 1, 0.5, cv::Scalar( 255, 0, 255 ) );
                }

                std::string pathTemp;
                pathTemp = folderPath;
                pathTemp += "/#" + graphPaths[i];

                // save image drawn with corners and labeled with indexes
                cv::imwrite( pathTemp, imageDrawCorners );
#endif
            }
#ifdef DEBUG_OUTPUT_INFO
            else // failed
            {
                std::cout << graphPaths[i] << " corner detect failed!" << std::endl;
            }
#endif

        }
#ifdef DEBUG_OUTPUT_INFO
        std::cout << "Corner detect done!" << std::endl
            << imageCountSuccess << " succeed! " << std::endl;
#endif


        if ( imageCountSuccess < 3 )
        {
#ifdef DEBUG_OUTPUT_INFO
            std::cout << "Calibrated success " << imageCountSuccess
                << " images, less than 3 images." << std::endl;
#endif
            return ;
        }
        else
        {
#ifdef DEBUG_OUTPUT_INFO
            std::cout << "Start calibration ..." << std::endl;
#endif
            cv::Point3f point3D;
            std::vector<cv::Point3f> objectPoints;
            std::vector<double> distCoeffs;
            std::vector<double> rotation;
            std::vector<double> translation;

            std::vector<std::vector<cv::Point3f>> seqObjectPoints;
            std::vector<std::vector<double>> seqRotation;
            std::vector<std::vector<double>> seqTranslation;
            cv::Mat_<double> cameraMatrix;

            // calibration pattern points in the calibration pattern coordinate space
            for ( int t=0; t<imageCountSuccess; t++ )
            {
                objectPoints.clear();
                for ( int i=0; i<boardSize.height; i++ )
                {
                    for ( int j=0; j<boardSize.width; j++ )
                    {
                        point3D.x = i * squareSize.width;
                        point3D.y = j * squareSize.height;
                        point3D.z = 0;
                        objectPoints.push_back(point3D);
                    }
                }
                seqObjectPoints.push_back(objectPoints);
            }

            double reprojectionError = calibrateCamera(
                seqObjectPoints,
                seqCorners,
                image_size,
                cameraMatrix,
                distCoeffs,
                seqRotation,
                seqTranslation,
                CV_CALIB_FIX_ASPECT_RATIO|CV_CALIB_FIX_PRINCIPAL_POINT );

#ifdef DEBUG_OUTPUT_INFO
            std::cout << "Calibration done!" << std::endl;
#endif
            // calculate the calibration pattern points with the camera model
            std::vector<cv::Mat_<double>> projectMats;

            for ( int i=0; i<imageCountSuccess; i++ )
            {
                cv::Mat_<double> R, T;
                // translate rotation vector to rotation matrix via Rodrigues transformation
                cv::Rodrigues( seqRotation[i], R );
                T = cv::Mat( cv::Matx31d(
                    seqTranslation[i][0],
                    seqTranslation[i][1],
                    seqTranslation[i][2]) );

                cv::Mat_<double> P = cameraMatrix * cv::Mat( cv::Matx34d(
                    R(0,0), R(0,1), R(0,2), T(0),
                    R(1,0), R(1,1), R(1,2), T(1),
                    R(2,0), R(2,1), R(2,2), T(2) ) );

                projectMats.push_back(P);
            }

            std::vector<cv::Point2d> PointSet;
            int pointNum = boardSize.width*boardSize.height;
            std::vector<cv::Point3d> objectClouds;
            for ( int i=0; i<pointNum; i++ )
            {
                PointSet.clear();
                for ( int j=0; j<imageCountSuccess; j++ )
                {
                    cv::Point2d tempPoint = seqCorners[j][i];
                    PointSet.push_back(tempPoint);
                }
                // calculate calibration pattern points
                cv::Point3d objectPoint = calAssist.triangulate(projectMats,PointSet);
                objectClouds.push_back(objectPoint);
            }
            std::string pathTemp_point;
            pathTemp_point = folderPath;
            pathTemp_point += "/point.txt";
            calAssist.save3dPoint(pathTemp_point,objectClouds);

            std::string pathTemp_calib;
            pathTemp_calib = folderPath;
            pathTemp_calib += "/calibration.txt";

            FILE* fp = fopen( pathTemp_calib.c_str(), "w" );
            fprintf( fp, "The average of re-projection error : %lf\n", reprojectionError );
            for ( int i=0; i<imageCountSuccess; i++ )
            {
                std::vector<cv::Point2f> errorList;
                cv::projectPoints(
                    seqObjectPoints[i],
                    seqRotation[i],
                    seqTranslation[i],
                    cameraMatrix,
                    distCoeffs,
                    errorList );

                corners.clear();
                corners = seqCorners[i];

                double meanError(0.0);
                for ( int j=0; j<corners.size(); j++ )
                {
                    meanError += std::sqrt((errorList[j].x - corners[j].x)*(errorList[j].x - corners[j].x) +
                        (errorList[j].y - corners[j].y)*(errorList[j].y - corners[j].y));
                }
                rotation.clear();
                translation.clear();

                rotation = seqRotation[i];
                translation = seqTranslation[i];
                fprintf( fp, "Re-projection of image %d：%lf\n", i+1, meanError/corners.size() );
                fprintf( fp, "Rotation vector :\n" );
                fprintf( fp, "%lf %lf %lf\n", rotation[0], rotation[1], rotation[2] );
                fprintf( fp, "Translation vector :\n" );
                fprintf( fp, "%lf %lf %lf\n\n", translation[0], translation[1], translation[2] );
            }
            fprintf( fp, "Camera internal matrix :\n" );
            fprintf( fp, "%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
                cameraMatrix(0,0), cameraMatrix(0,1), cameraMatrix(0,2),
                cameraMatrix(1,0), cameraMatrix(1,1), cameraMatrix(1,2),
                cameraMatrix(2,0), cameraMatrix(2,1), cameraMatrix(2,2));
            fprintf( fp,"Distortion coefficient :\n" );
            for ( int k=0; k<distCoeffs.size(); k++)
                fprintf( fp, "%lf ", distCoeffs[k] );
#ifdef DEBUG_OUTPUT_INFO
            std::cout << "Results are saved!" << std::endl;
#endif
        }
    }
}
