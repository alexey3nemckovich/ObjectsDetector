#include "stdafx.h"
#include "MostMatchingValueClassifier.h"


string MostMatchingValueClassifier::GetName() const
{
    return "MostMatchingValueClassifier";
}


vector<ObjectIteratorList> MostMatchingValueClassifier::Classify(const ObjectsList& objects) const
{
    auto maxValResultsMap = std::move(FindMostMatchingTemplateMap(objects));
    vector<ObjectIteratorList> objectsGroups;

    while (maxValResultsMap.size() > 0)
    {
        objectsGroups.push_back(std::move(
            RemoveObjectsGroup(maxValResultsMap)
        ));
    }

    return objectsGroups;
}


ObjectIteratorList MostMatchingValueClassifier::RemoveObjectsGroup(
    map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>>& m
)
{
    ObjectIteratorList group;

    //Find bearing group elements
    auto currEl = m.begin()->first;
    while (true)
    {
        pair<ObjectsList::const_iterator, double> currElMaxValInfo(std::move(
            m[currEl]
        ));

        group.push_back(currEl);

        if (currElMaxValInfo.second < 0.5)
        {
            break;
        }
        else
        {
            if (group.end() == find(group.begin(), group.end(), currElMaxValInfo.first))
            {
                currEl = currElMaxValInfo.first;
            }
            else
            {
                break;
            }
        }
    }

    //Find all group elements, next logic:
    //
    //[1]if element is similar to some group element than
    //   it's group element
    //
    //[2]if found new group elements than you should
    //   check again elements, because they can be similar
    //   to new group elements
    while (true)
    {
        ObjectIteratorList newGroupElements;

        for (auto it = m.begin(); m.end() != it; it++)
        {
            if (group.end() != find(group.begin(), group.end(), it->first))
            {
                continue;
            }

            if (group.end() != find(group.begin(), group.end(), it->second.first) && it->second.second >= 0.5)
            {
                newGroupElements.push_back(it->first);
            }
        }

        if (newGroupElements.size() > 0)
        {
            group.insert(group.end(), newGroupElements.begin(), newGroupElements.end());
        }
        else
        {
            break;
        }
    }

    //Erase group elements from future considering
    for (auto it = group.begin(); group.end() != it; it++)
    {
        m.erase(*it);
    }

    return group;
}


map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> MostMatchingValueClassifier::FindMostMatchingTemplateMap(
    const ObjectsList& objects
)
{
    auto templateMatсhingValueMap = FindTemplateMatchingValueMap(objects);

    map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> maxValResultsMap;
    for (auto it = templateMatсhingValueMap.begin(); templateMatсhingValueMap.end() != it; it++)
    {
        auto& currObj = it->first;
        auto& currObjTemplateMap = templateMatсhingValueMap[currObj];
        auto& currobjTemplateMatchMaxValInfo = maxValResultsMap[currObj];

        currobjTemplateMatchMaxValInfo.first = currObjTemplateMap.begin()->first;
        currobjTemplateMatchMaxValInfo.second = currObjTemplateMap.begin()->second;

        for (auto templIt = ++currObjTemplateMap.begin(); currObjTemplateMap.end() != templIt; templIt++)
        {
            if (templIt->second > currobjTemplateMatchMaxValInfo.second)
            {
                currobjTemplateMatchMaxValInfo.first = templIt->first;
                currobjTemplateMatchMaxValInfo.second = templIt->second;
            }
        }
    }

    return maxValResultsMap;
}
