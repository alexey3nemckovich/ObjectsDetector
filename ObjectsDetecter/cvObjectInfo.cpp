#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    int ObjectInfo::nextObjectID = 0;

    ObjectInfo::ObjectInfo()
    {
        sourceImageRect.x = UNDEFINED_COORDINATE;
        sourceImageRect.y = UNDEFINED_COORDINATE;

        id = nextObjectID;
        name = cv::format("object%d.jpg", id);

        nextObjectID++;
    }

    bool operator==(const ObjectInfo& a, const ObjectInfo& b)
    {
        if (a.id == b.id)
        {
            return true;
        }

        return false;
    }

    void SaveObjectsImages(const ObjectsList& objects)
    {
        vector<int> compression_params;
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        for (int i = 0; i < objects.size(); i++)
        {
            imwrite(objects[i].name, objects[i].image, compression_params);
        }
    }

}
