#include "stdafx.h"
#include "CriterionClassifier.h"


string CritertionClassifier::GetName() const
{
    return "CritertionClassifier";
}


vector<ObjectIteratorList> CritertionClassifier::Classify(const ObjectsList& objects) const
{
    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> templateMatchingValMap(std::move(
        FindTemplateMatchingValueMap(objects)
    ));
    map<ObjectIterator, double> objectsClassificationParamMap(std::move(
        FindObjectsClassificationParamMap(templateMatchingValMap)
    ));

#ifdef _DEBUG
    for (auto it = objectsClassificationParamMap.begin(); objectsClassificationParamMap.end() != it; it++)
    {
        imshow(format("%f", it->second), it->first->image);
        waitKey();
    }
#endif

    vector<ObjectIteratorList> objectsGroups;
    while (objectsClassificationParamMap.size() > 0)
    {
        objectsGroups.push_back(std::move(
            RemoveObjectsGroup(objectsClassificationParamMap)
        ));
    }

    return objectsGroups;
}


ObjectIteratorList CritertionClassifier::RemoveObjectsGroup(map<ObjectIterator, double>& m)
{
    ObjectIteratorList group;

    double groupVal = m.begin()->second;
    double groupSumVal = groupVal;

    group.push_back(m.begin()->first);
    m.erase(m.begin());

    for (auto it = m.begin(); m.end() != it; it++)
    {
        if (abs(it->second - groupVal) < 0.02)
        {
            group.push_back(it->first);

            groupSumVal += it->second;
            groupVal = groupSumVal / group.size();
        }
    }

    for (auto it = group.begin(); group.end() != it; it++)
    {
        m.erase(*it);
    }

    return group;
}


map<ObjectIterator, double> CritertionClassifier::FindObjectsClassificationParamMap(
    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap
)
{
    map<ObjectIterator, double> res;

    int maxRowsCount = 0;
    int maxColsCount = 0;

    for (auto it = templateMatchingValMap.begin(); templateMatchingValMap.end() != it; it++)
    {
        auto& img = it->first->image;

        if (img.cols > maxColsCount)
        {
            maxColsCount = img.cols;
        }

        if (img.rows > maxRowsCount)
        {
            maxRowsCount = img.rows;
        }
    }

    for (auto it = templateMatchingValMap.begin(); templateMatchingValMap.end() != it; it++)
    {
        auto& currObj = it->first;
        auto& currObjTemplateMap = templateMatchingValMap[currObj];

        for (auto templIt = ++currObjTemplateMap.begin(); currObjTemplateMap.end() != templIt; templIt++)
        {
            res[currObj] += templIt->second;
        }

        res[currObj] /= currObjTemplateMap.size();

        res[currObj] = (res[currObj] + (double)currObj->image.cols / maxColsCount + (double)currObj->image.rows / maxRowsCount) / 3;
    }
    return res;
}
