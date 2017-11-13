#include "stdafx.h"
#include "ImageProcesser.h"


map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> ImageProcesser::ObjectsClassifier::FindTemplateMatchingValueMap(const ObjectsList& objects)
{
    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> res;

    for (auto it = objects.begin(); objects.end() != it; it++)
    {
        res[it] = std::move(FindTemplateMatchingValueMapFor(*it, objects));
    }
    return res;
}


map<ObjectsList::const_iterator, double> ImageProcesser::ObjectsClassifier::FindTemplateMatchingValueMapFor(const ObjectInfo& obj, const ObjectsList& objects)
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
