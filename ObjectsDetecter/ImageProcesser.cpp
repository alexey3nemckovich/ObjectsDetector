#include "stdafx.h"
#include "ImageProcesser.h"


ImageProcesser& ImageProcesser::GetInstance()
{
    static ImageProcesser instance;
    return instance;
}


ImageProcesser::ImageProcesser()
{
    _objectsClassifyingAlgorithmsMap[Algorithm::Alg1] = ObjectsClassifyingAlg1;
    _objectsClassifyingAlgorithmsMap[Algorithm::Alg2] = ObjectsClassifyingAlg2;
    _objectsClassifyingAlgorithmsMap[Algorithm::Alg3] = ObjectsClassifyingAlg3;
}


const ImageProcessResult& ImageProcesser::ProcessImage(cv::Mat& image)
{
    _lastImageProcessingResult = cvutils::ProcessImage(image, _objectsClassifyingAlgorithm);
    return _lastImageProcessingResult;
}


void ImageProcesser::SetImageProcessingAlgorithm(Algorithm alg)
{
    _objectsClassifyingAlgorithm = _objectsClassifyingAlgorithmsMap[alg];
}


const ImageProcessResult& ImageProcesser::GetLastImageProcessResult() const
{
    return _lastImageProcessingResult;
}
