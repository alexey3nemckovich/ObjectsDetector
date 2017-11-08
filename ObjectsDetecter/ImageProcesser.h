#pragma once
#include <map>
#include "cvUtilities.h"
using namespace cvutils;


class ImageProcesser
{
public:
    static ImageProcesser& GetInstance();

    enum class ObjectsClassifyingAlgorithmName
    {
        Alg1,
        Alg2,
        Alg3
    };

public:
    ImageProcesser();

public:
    const ImageProcessResult& ProcessImage(cv::Mat&);
    void SetImageProcessingAlgorithm(ObjectsClassifyingAlgorithmName);

public:
    const ImageProcessResult& GetLastImageProcessResult() const;

private:
    ImageProcessResult _lastImageProcessingResult;
    ObjectsClassifyingAlgorithm _objectsClassifyingAlgorithm;
    std::map<ObjectsClassifyingAlgorithmName, ObjectsClassifyingAlgorithm> _objectsClassifyingAlgorithmsMap;
};
