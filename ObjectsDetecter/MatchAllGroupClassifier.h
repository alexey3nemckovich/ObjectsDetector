#pragma once
#include "ImageProcesser.h"


class MatchAllGroupClassifier : public ImageProcesser::ObjectsClassifier
{
public:
    virtual string GetName() const override;
    virtual vector<ObjectIteratorList> Classify(const ObjectsList& objects) const override;

private:
    static ObjectIteratorList RemoveObjectsGroup(
        map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& m
    );
};
