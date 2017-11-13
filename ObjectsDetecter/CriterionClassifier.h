#pragma once
#include "ImageProcesser.h"


class CritertionClassifier : public ImageProcesser::ObjectsClassifier
{
public:
    virtual string GetName() const override;
    virtual vector<ObjectIteratorList> Classify(const ObjectsList& objects) const override;

private:
    static ObjectIteratorList RemoveObjectsGroup(map<ObjectIterator, double>& m);
    static map<ObjectIterator, double> FindObjectsClassificationParamMap(
        map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap
    );
};
