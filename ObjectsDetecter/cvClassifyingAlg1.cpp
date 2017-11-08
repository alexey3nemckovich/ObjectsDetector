#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    ObjectIteratorList RemoveObjectsGroup(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& m);

    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects);

    vector<ObjectIteratorList> ObjectsClassifyingAlg1(const ObjectsList& objects)
    {
        auto maxValResultsMap = std::move(FindTemplateMatchingValueMap(objects));
        vector<ObjectIteratorList> objectsGroups;

        while (maxValResultsMap.size() > 0)
        {
            objectsGroups.push_back(std::move(
                RemoveObjectsGroup(maxValResultsMap)
            ));
        }

        return objectsGroups;
    }

    ObjectIteratorList RemoveObjectsGroup(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& m)
    {
        ObjectIteratorList group;

        group.push_back(m.begin()->first);

        while (true)
        {
            ObjectIteratorList newGroupElements;

            for (auto it = m.begin(); m.end() != it; it++)
            {
                //Check if element is already in group
                if (group.end() != find(group.begin(), group.end(), it->first))
                {
                    continue;
                }

                //Check if element is similar to all group elements
                bool isNewGroupElement = true;
                for (auto it2 = group.begin(); group.end() != it2; it2++)
                {
                    if (m[it->first][*it2] < 0.5)
                    {
                        isNewGroupElement = false;
                        break;
                    }
                }

                if (isNewGroupElement)
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

        //Remove elements from group, that are no similar to all group elements
        while (true)
        {
            ObjectIteratorList noGroupElements;

            for (int i = 0; i < group.size(); i++)
            {
                for (int i2 = 0; i2 < group.size(); i2++)
                {
                    if (i == i2)
                    {
                        continue;
                    }

                    if (noGroupElements.end() != find(noGroupElements.begin(), noGroupElements.end(), group[i]))
                    {
                        continue;
                    }

                    if (m[group[i]][group[i2]] < 0.5)
                    {
                        noGroupElements.push_back(group[i]);
                        break;
                    }
                }
            }

            if (noGroupElements.size() > 0)
            {
                for (int i = 0; i < noGroupElements.size(); i++)
                {
                    group.erase(
                        find(group.begin(), group.end(), noGroupElements[i])
                    );
                }
            }
            else
            {
                break;
            }
        }

        if (0 == group.size())
        {
            group.push_back(m.begin()->first);
        }

        for (auto& groupEl : group)
        {
            m.erase(groupEl);
        }

        return group;
    }

}
