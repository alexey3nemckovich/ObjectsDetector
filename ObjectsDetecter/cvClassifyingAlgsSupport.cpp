#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    map<ObjectsList::const_iterator, double> FindTemplateMatchingValueMapFor(const ObjectInfo& obj, const ObjectsList& objects);

    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects);

    map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> FindMostMatchingTemplateMap(const ObjectsList& objects);

    map<ObjectIterator, double> FindObjectsClassificationParamMap(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap);

    map<ObjectIterator, double> FindObjectsClassificationParamMap(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap)
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

    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects)
    {
        map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> res;

        for (auto it = objects.begin(); objects.end() != it; it++)
        {
            res[it] = std::move(FindTemplateMatchingValueMapFor(*it, objects));
        }
        return res;
    }

    map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> FindMostMatchingTemplateMap(const ObjectsList& objects)
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

    map<ObjectsList::const_iterator, double> FindTemplateMatchingValueMapFor(const ObjectInfo& obj, const ObjectsList& objects)
    {
        const Mat& img = obj.image;
        map<ObjectsList::const_iterator, double> res;

        for (auto templateIt = objects.begin(); objects.end() != templateIt; templateIt++)
        {
            if (*templateIt == obj)
            {
                continue;
            }

            const Mat& templ = templateIt->image;

            Mat result;
            double minVal, maxVal;
            Point minLoc, maxLoc;

            if (img.rows < templ.rows ||
                img.cols < templ.cols)
            {
                int vertBorder = 0;
                int horzBorder = 0;

                Mat sizedImage;
                img.copyTo(sizedImage);

                if (sizedImage.rows < templ.rows)
                {
                    ResizeImageToRowsCount(templ.rows, sizedImage);
                }

                if (sizedImage.cols < templ.cols)
                {
                    ResizeImageToColsCount(templ.cols, sizedImage);
                }

                int result_cols = sizedImage.cols - templ.cols + 1;
                int result_rows = sizedImage.rows - templ.rows + 1;

                result.create(result_rows, result_cols, CV_32FC1);
                matchTemplate(sizedImage, templ, result, TM_CCOEFF_NORMED);
            }
            else
            {
                int result_cols = img.cols - templ.cols + 1;
                int result_rows = img.rows - templ.rows + 1;

                result.create(result_rows, result_cols, CV_32FC1);
                matchTemplate(img, templ, result, TM_CCOEFF_NORMED);
            }

            //normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
            minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            res[templateIt] = maxVal;
        }

        return res;
    }

}
