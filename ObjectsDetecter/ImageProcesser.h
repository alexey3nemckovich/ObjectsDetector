#pragma once
#include <map>
#include <memory>
#include "cvUtilities.h"
using namespace cvutils;


class ImageProcesser
{
public:
    static ImageProcesser& GetInstance();
    class ObjectsClassifier;

public:
    ImageProcesser() = default;

public:
    vector<string> GetListClassifiersNames() const;
    const ImageProcessResult& GetLastImageProcessResult() const;

public:
    const ImageProcessResult& ProcessImage(cv::Mat&);
    void SetObjectsClassifierByName(const string& name);

private:
    class ObjectsClassifierFactory;

private:
    ImageProcessResult _lastImageProcessingResult;
    std::shared_ptr<ObjectsClassifier> _objectsClassifier;
};


class NamedObject
{
public:
    virtual string GetName() const = 0;
};


class ImageProcesser::ObjectsClassifier : public NamedObject
{
public:
    typedef shared_ptr<ObjectsClassifier> Ptr;

public:
    virtual vector<ObjectIteratorList> Classify(const ObjectsList& objects) const = 0;

protected:
    static map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects);

private:
    static map<ObjectsList::const_iterator, double> FindTemplateMatchingValueMapFor(const ObjectInfo& obj, const ObjectsList& objects);
};


class ImageProcesser::ObjectsClassifierFactory
{
public:
    static ObjectsClassifierFactory& GetInstance();

public:
    ObjectsClassifierFactory();

public:
    vector<string> GetListClassifiersNames();
    ObjectsClassifier::Ptr GetClassifierByName(const string& name);

private:
    map<string, ObjectsClassifier::Ptr> _classifiersMap;
};
