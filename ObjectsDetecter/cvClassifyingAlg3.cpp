#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    ObjectIteratorList RemoveObjectsGroup(map<ObjectIterator, double>& m);

    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects);

    map<ObjectIterator, double> FindObjectsClassificationParamMap(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap);

    vector<ObjectIteratorList> ObjectsClassifyingAlg3(const ObjectsList& objects)
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

    ObjectIteratorList RemoveObjectsGroup(map<ObjectIterator, double>& m)
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

}
