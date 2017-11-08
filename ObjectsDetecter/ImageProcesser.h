#pragma once
#include "cvUtilities.h"
using namespace cvutils;


class ImageProcesser
{
public:
    static ImageProcesser& GetInstance();

public:
    const ImageProcessResult& ProcessImage(cv::Mat&);

public:
    const ImageProcessResult& GetLastImageProcessResult() const;

private:
    ImageProcessResult _lastImageProcessingResult;
};
