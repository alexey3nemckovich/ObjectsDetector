#pragma once
#include "ImageProcesser.h"


class MostMatchingValueClassifier : public ImageProcesser::ObjectsClassifier
{
public:
    virtual string GetName() const override;
    virtual vector<ObjectIteratorList> Classify(const ObjectsList& objects) const override;

private:
    static ObjectIteratorList RemoveObjectsGroup(
        map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>>& m
    );

    static map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> FindMostMatchingTemplateMap(
        const ObjectsList& objects
    );
};
