#pragma once
#include <map>
#include "cvUtilities.h"
using namespace cvutils;


class ImageProcesser
{
public:
    static ImageProcesser& GetInstance();

    enum class Algorithm
    {
        Alg1,
        Alg2,
        Alg3
    };

public:
    ImageProcesser();

public:
    const ImageProcessResult& ProcessImage(cv::Mat&);
    void SetImageProcessingAlgorithm(Algorithm);

public:
    const ImageProcessResult& GetLastImageProcessResult() const;

private:
    ImageProcessResult _lastImageProcessingResult;
    ObjectsClassifyingAlgorithm _objectsClassifyingAlgorithm;
    std::map<Algorithm, ObjectsClassifyingAlgorithm> _objectsClassifyingAlgorithmsMap;
};
