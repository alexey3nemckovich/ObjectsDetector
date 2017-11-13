#include "stdafx.h"
#include "ImageProcesser.h"
#include "CriterionClassifier.h"
#include "MatchAllGroupClassifier.h"
#include "MostMatchingValueClassifier.h"


ImageProcesser::ObjectsClassifierFactory& ImageProcesser::ObjectsClassifierFactory::GetInstance()
{
    static ObjectsClassifierFactory instance;
    return instance;
}


ImageProcesser::ObjectsClassifierFactory::ObjectsClassifierFactory()
{
    auto createAndAddClassifierToMap = [this](ImageProcesser::ObjectsClassifier* classifier) {
        _classifiersMap[classifier->GetName()] = ImageProcesser::ObjectsClassifier::Ptr(classifier);
    };

    createAndAddClassifierToMap(new CritertionClassifier());
    createAndAddClassifierToMap(new MatchAllGroupClassifier());
    createAndAddClassifierToMap(new MostMatchingValueClassifier());
}


vector<string> ImageProcesser::ObjectsClassifierFactory::GetListClassifiersNames()
{
    vector<string> names;

    for (auto mapPair : _classifiersMap)
    {
        names.push_back(mapPair.first);
    }

    return names;
}


ImageProcesser::ObjectsClassifier::Ptr ImageProcesser::ObjectsClassifierFactory::GetClassifierByName(const string& name)
{
    return _classifiersMap[name];
}
