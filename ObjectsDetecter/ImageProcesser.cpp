#include "stdafx.h"
#include "ImageProcesser.h"


ImageProcesser& ImageProcesser::GetInstance()
{
    static ImageProcesser instance;
    return instance;
}


const ImageProcessResult& ImageProcesser::ProcessImage(cv::Mat& image)
{
    _lastImageProcessingResult = cvutils::ProcessImage(image);
    return _lastImageProcessingResult;
}


const ImageProcessResult& ImageProcesser::GetLastImageProcessResult() const
{
    return _lastImageProcessingResult;
}
