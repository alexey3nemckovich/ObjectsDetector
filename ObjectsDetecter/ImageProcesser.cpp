#include "stdafx.h"
#include "ImageProcesser.h"


ImageProcesser& ImageProcesser::GetInstance()
{
    static ImageProcesser instance;
    return instance;
}


const ImageProcessResult& ImageProcesser::ProcessImage(cv::Mat& image)
{
    constexpr int imageMaxWidth = 800;
    constexpr int imageMaxHeight = 500;

    BoundImage(image, imageMaxWidth, imageMaxHeight);

    ImageProcessResult res(image);
    res.detectedObjects = SegmentObjects(image);

    for (int i = 0; i < res.detectedObjects.size(); i++)
    {
        rectangle(res.objectsImage, res.detectedObjects[i].sourceImageRect, GetRandomColor(), 2);
    }

    int dy = 20;
    int dx = 100;
    cv::Point p(10, dy);
    auto& objGroups = res.detectedObjectsGroups = std::move(_objectsClassifier->Classify(res.detectedObjects));
    for (int i = 0; i < objGroups.size(); i++)
    {
        auto typeColor = GetRandomColor();

        Point rightPoint(p.x + dx, p.y);
        line(res.groupsImage, p, rightPoint, typeColor, 2);
        rightPoint.x += 5;
        putText(res.groupsImage, format("group%d", i), rightPoint, FONT_HERSHEY_TRIPLEX, 0.8, typeColor, 2);
        p.y += dy;

        for (auto groupObjectsIt = objGroups[i].begin(); objGroups[i].end() != groupObjectsIt; groupObjectsIt++)
        {
            rectangle(res.groupsImage, (*groupObjectsIt)->sourceImageRect, typeColor, 2);
        }
    }

    _lastImageProcessingResult = res;
    return res;
}


void ImageProcesser::SetObjectsClassifierByName(const string& name)
{
    _objectsClassifier = 
        ImageProcesser::ObjectsClassifierFactory::GetInstance().GetClassifierByName(name);
}


vector<string> ImageProcesser::GetListClassifiersNames() const
{
    return ImageProcesser::ObjectsClassifierFactory::GetInstance().GetListClassifiersNames();
}


const ImageProcessResult& ImageProcesser::GetLastImageProcessResult() const
{
    return _lastImageProcessingResult;
}
