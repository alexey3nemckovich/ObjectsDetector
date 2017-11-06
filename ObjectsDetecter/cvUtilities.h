#pragma once
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;


namespace cvutils
{
    
    void ResizeImageToRowsCount(int rowsCount, Mat& image);

    void ResizeImageToColsCount(int colsCount, Mat& image);

    void BoundImage(Mat& image, int width, int height);

    cv::Vec3b GetRandomColor();

    auto ConvertCVMatToBMP(const cv::Mat& frame)->HBITMAP;

    struct ObjectInfo
    {
    public:
        ObjectInfo();

    public:
        Mat image;
        Rect sourceImageRect;

    public:
        cv::String name;

    private:
        int id = 0;
        static int nextObjectID;
    };

    typedef vector<ObjectInfo> ObjectsList;
    typedef vector<ObjectsList::iterator> PobjectsList;
    typedef map<ObjectsList::const_iterator, vector<ObjectsList::const_iterator>> ObjectsGroupsMap;

    void SaveObjectImages(const ObjectsList& objects);

    struct ImageProcessResult
    {
        ImageProcessResult() = default;
        ImageProcessResult(const cv::Mat&);
        Mat typesImage;
        Mat objectsImage;
        ObjectsList detectedObjects;
        ObjectsGroupsMap detectedObjectsGroupsMap;
    };

    ImageProcessResult ProcessImage(const cv::Mat& img);

}
