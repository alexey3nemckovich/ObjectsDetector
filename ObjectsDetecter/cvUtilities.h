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

        friend bool operator==(const ObjectInfo& a, const ObjectInfo& b);
    };

    bool operator==(const ObjectInfo& a, const ObjectInfo& b);

    typedef vector<ObjectInfo> ObjectsList;

    typedef ObjectsList::const_iterator ObjectIterator;
    typedef vector<ObjectIterator> ObjectIteratorList;

    void SaveObjectImages(const ObjectsList& objects);

    struct ImageProcessResult
    {
        ImageProcessResult() = default;
        ImageProcessResult(const cv::Mat&);
        Mat groupsImage;
        Mat objectsImage;
        ObjectsList detectedObjects;
        vector<ObjectIteratorList> detectedObjectsGroups;
    };

    ImageProcessResult ProcessImage(cv::Mat& img, bool useSharpening = true);

}
