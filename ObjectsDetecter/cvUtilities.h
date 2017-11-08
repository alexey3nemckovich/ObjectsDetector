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
    //Image mat processing utils
    void ResizeImageToRowsCount(int rowsCount, Mat& image);

    void ResizeImageToColsCount(int colsCount, Mat& image);

    void BoundImage(Mat& image, int width, int height);

    cv::Vec3b GetRandomColor();

    auto ConvertCVMatToBMP(const cv::Mat& frame)->HBITMAP;

    //Computer vision utils
    struct ObjectInfo
    {
    public:
        ObjectInfo();

    public:
        Mat image;
        Rect sourceImageRect;

    public:
        static const int UNDEFINED_COORDINATE = -1;

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

    void SaveObjectsImages(const ObjectsList& objects);

    typedef vector<ObjectIteratorList> (*ObjectsClassifyingAlgorithm)(const ObjectsList& objects);

    vector<ObjectIteratorList> ObjectsClassifyingAlg1(const ObjectsList& objects);

    vector<ObjectIteratorList> ObjectsClassifyingAlg2(const ObjectsList& objects);

    vector<ObjectIteratorList> ObjectsClassifyingAlg3(const ObjectsList& objects);

    struct ImageProcessResult
    {
        ImageProcessResult() = default;
        ImageProcessResult(const cv::Mat&);
        Mat groupsImage;
        Mat objectsImage;
        ObjectsList detectedObjects;
        vector<ObjectIteratorList> detectedObjectsGroups;
    };

    ImageProcessResult ProcessImage(cv::Mat& img, ObjectsClassifyingAlgorithm classifyingAlgorithm);

}
