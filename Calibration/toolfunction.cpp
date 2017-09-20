// toolFunction.cpp
#include "toolFunction.h"

std::vector<std::string> CalibrationAssist::get_filelist( std::string foldname )
{
    foldname += "/*.*";
    const char * mystr=foldname.c_str();
    std::vector<std::string> flist;
    std::string lineStr;
    std::vector<std::string> extendName;
    extendName.push_back("jpg");
    extendName.push_back("JPG");
    extendName.push_back("bmp");
    extendName.push_back("png");
    extendName.push_back("gif");

    HANDLE file;
    WIN32_FIND_DATA fileData;
    char line[1024];
    wchar_t fn[1000];
    mbstowcs( fn, mystr, 999 );
    file = FindFirstFile( fn, &fileData );
    FindNextFile( file, &fileData );
    while(FindNextFile( file, &fileData ))
    {
        wcstombs( line, (const wchar_t*)fileData.cFileName, 259);
        lineStr = line;
        // remove the files which are not images
        for (int i = 0; i < 4; i ++)
        {
            if (lineStr.find(extendName[i]) < 999)
            {
                flist.push_back(lineStr);
                break;
            }
        }
    }
    return flist;
}


cv::Point3d CalibrationAssist::triangulate(
    std::vector<cv::Mat_<double>> &ProjectMats,
    std::vector<cv::Point2d> &imagePoints)
{
    int i,j;
    std::vector<cv::Point2d> pointSet;
    int frameSum = ProjectMats.size();
    cv::Mat A(2*frameSum,3,CV_32FC1);
    cv::Mat B(2*frameSum,1,CV_32FC1);
    cv::Point2d u,u1;
    cv::Mat_<double> P;
    cv::Mat_<double> rowA1,rowA2,rowB1,rowB2;
    int k = 0;
    for ( i = 0; i < frameSum; i++ )     //get the coefficient matrix A and B
    {
        u = imagePoints[i];
        P = ProjectMats[i];
        cv::Mat( cv::Matx13d(
            u.x*P(2,0)-P(0,0),
            u.x*P(2,1)-P(0,1),
            u.x*P(2,2)-P(0,2) ) ).copyTo( A.row(k) );

        cv::Mat( cv::Matx13d(
            u.y*P(2,0)-P(1,0),
            u.y*P(2,1)-P(1,1),
            u.y*P(2,2)-P(1,2) ) ).copyTo( A.row(k+1) );

        cv::Mat rowB1( 1, 1, CV_32FC1, cv::Scalar( -(u.x*P(2,3)-P(0,3)) ) );
        cv::Mat rowB2( 1, 1, CV_32FC1, cv::Scalar(-(u.y*P(2,3)-P(1,3)) ) );
        rowB1.copyTo( B.row(k) );
        rowB2.copyTo( B.row(k+1) );
        k += 2;
    }
    cv::Mat X;
    cv::solve( A, B, X, DECOMP_SVD );
    return Point3d(X);
}

void CalibrationAssist::save3dPoint( std::string path_, std::vector<cv::Point3d> &Point3dLists)
{
    const char * path = path_.c_str();
    FILE* fp = fopen( path, "w" );
    for ( int i = 0; i < Point3dLists.size(); i ++)
    {
        //      fprintf(fp,"%d ",i);
        fprintf( fp, "%lf %lf %lf\n",
            Point3dLists[i].x, Point3dLists[i].y, Point3dLists[i].z);
    }
    fclose(fp);
#if 1
    std::cout << "clouds of points are saved!" << std::endl;
#endif
}
