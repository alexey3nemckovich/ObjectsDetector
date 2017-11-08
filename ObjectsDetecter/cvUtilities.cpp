﻿#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    void ResizeImageToRowsCount(int rowsCount, Mat& image)
    {
        double f = (double)rowsCount / image.rows;
        resize(image, image, Size(), f, f);
    }

    void ResizeImageToColsCount(int colsCount, Mat& image)
    {
        double f = (double)colsCount / image.cols;
        resize(image, image, Size(), f, f);
    }

    void BoundImage(Mat& image, int width, int height)
    {
        if (image.rows > height)
        {
            ResizeImageToRowsCount(height, image);
        }

        if (image.cols > width)
        {
            ResizeImageToColsCount(width, image);
        }
    }

    cv::Vec3b GetRandomColor()
    {
        int b = cv::theRNG().uniform(0, 255);
        int g = cv::theRNG().uniform(0, 255);
        int r = cv::theRNG().uniform(0, 255);
        return cv::Vec3b((uchar)b, (uchar)g, (uchar)r);
    }

    int CvDepthToBitsCount(int cvDepth)
    {
        ushort bitsCount;
        switch (cvDepth)
        {
        case CV_8U:
        case CV_8S:
            bitsCount = 8;
            break;
        case CV_16U:
        case CV_16S:
            bitsCount = 16;
            break;
        case CV_32S:
        case CV_32F:
            bitsCount = 32;
            break;
        case CV_64F:
            bitsCount = 64;
            break;
        default:
            bitsCount = 0;
            break;
        }
        return bitsCount;
    }

    auto ConvertCVMatToBMP(const cv::Mat& frame) -> HBITMAP
    {
        auto convertOpenCVBitDepthToBits = [](const int32_t value)
        {
            auto regular = 0u;

            switch (value)
            {
            case CV_8U:
            case CV_8S:
                regular = 8u;
                break;

            case CV_16U:
            case CV_16S:
                regular = 16u;
                break;

            case CV_32S:
            case CV_32F:
                regular = 32u;
                break;

            case CV_64F:
                regular = 64u;
                break;

            default:
                regular = 0u;
                break;
            }

            return regular;
        };

        auto imageSize = frame.size();
        assert(imageSize.width && "invalid size provided by frame");
        assert(imageSize.height && "invalid size provided by frame");

        if (imageSize.width && imageSize.height)
        {
            auto headerInfo = BITMAPINFOHEADER{};
            ZeroMemory(&headerInfo, sizeof(headerInfo));

            headerInfo.biSize = sizeof(headerInfo);
            headerInfo.biWidth = imageSize.width;
            headerInfo.biHeight = -(imageSize.height); // negative otherwise it will be upsidedown
            headerInfo.biPlanes = 1;// must be set to 1 as per documentation frame.channels();

            const auto bits = convertOpenCVBitDepthToBits(frame.depth());
            headerInfo.biBitCount = frame.channels() * bits;

            auto bitmapInfo = BITMAPINFO{};
            ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));

            bitmapInfo.bmiHeader = headerInfo;
            bitmapInfo.bmiColors->rgbBlue = 0;
            bitmapInfo.bmiColors->rgbGreen = 0;
            bitmapInfo.bmiColors->rgbRed = 0;
            bitmapInfo.bmiColors->rgbReserved = 0;

            auto dc = GetDC(nullptr);
            assert(dc != nullptr && "Failure to get DC");
            auto bmp = CreateDIBitmap(dc,
                &headerInfo,
                CBM_INIT,
                frame.data,
                &bitmapInfo,
                DIB_RGB_COLORS);
            assert(bmp != nullptr && "Failure creating bitmap from captured frame");

            return bmp;
        }
        else
        {
            return nullptr;
        }
    }

    ImageProcessResult::ImageProcessResult(const cv::Mat& mat)
    {
        mat.copyTo(groupsImage);
        mat.copyTo(objectsImage);
    }

    ObjectsList SegmentObjects(const Mat& sourceImage);

    vector<ObjectIteratorList> FindObjectsGroups(const ObjectsList& objects);
    
    void BoundImage(Mat& image)
    {
        constexpr int imageMaxWidth = 800;
        constexpr int imageMaxHeight = 800;

        if (image.rows > 500)
        {
            ResizeImageToRowsCount(500, image);
        }

        if (image.cols > 800)
        {
            ResizeImageToColsCount(800, image);
        }
    }

    ImageProcessResult ProcessImage(cv::Mat& img)
    {
        BoundImage(img);

        ImageProcessResult res(img);
        res.detectedObjects = SegmentObjects(img);

        for (int i = 0; i < res.detectedObjects.size(); i++)
        {
            rectangle(res.objectsImage, res.detectedObjects[i].sourceImageRect, GetRandomColor(), 2);
        }

        int dy = 20;
        int dx = 100;
        cv::Point p(10, dy);
        auto& objGroups = res.detectedObjectsGroups = std::move(FindObjectsGroups(res.detectedObjects));
        for (int i = 0; i < objGroups.size(); i++)
        {
            auto typeColor = GetRandomColor();

            Point rightPoint(p.x + dx, p.y);
            line(res.groupsImage, p, rightPoint, typeColor, 2);
            rightPoint.x += 5;
            putText(res.groupsImage, format("group%d", i), rightPoint, FONT_HERSHEY_TRIPLEX, 0.8, typeColor, 2);
            p.y += dy;

            for (auto groupObjectsIt = objGroups[i].begin(); objGroups[i].end() != groupObjectsIt; groupObjectsIt++)
            {
                rectangle(res.groupsImage, (*groupObjectsIt)->sourceImageRect, typeColor, 2);
            }
        }

        return res;
    }

    constexpr int UNDEFINED = -1;

    int ObjectInfo::nextObjectID = 0;

    ObjectInfo::ObjectInfo()
    {
        sourceImageRect.x = UNDEFINED;
        sourceImageRect.y = UNDEFINED;

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

    ObjectsList SegmentObjects(const Mat& sourceImage)
    {
        Mat image;
        sourceImage.copyTo(image);

        ///Get binary image from source
        Mat binaryImage;
        cvtColor(image, binaryImage, CV_BGR2GRAY);
        threshold(binaryImage, binaryImage, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

        if (Vec3b(255, 255, 255) == binaryImage.at<Vec3b>(5, 5))
        {
            threshold(binaryImage, binaryImage, 40, 255, CV_THRESH_BINARY_INV);
        }

#ifdef _DEBUG
        imshow("Binary image", binaryImage);
        waitKey();
#endif

        ///Distance transform image
        Mat distanceTransformedImage;
        distanceTransform(binaryImage, distanceTransformedImage, CV_DIST_L2, 3);
#ifdef _DEBUG
        imshow("Distance transformed image", distanceTransformedImage);
        waitKey();
#endif

        ////Normalize distance transformed image to threshold it
        normalize(distanceTransformedImage, distanceTransformedImage, 0.0, 1.0, NORM_MINMAX);
#ifdef _DEBUG
        imshow("Normalized distance transformed image", distanceTransformedImage);
        waitKey();
#endif

        ///Threshold normalized distance transformed image to threshold it
        threshold(distanceTransformedImage, distanceTransformedImage, 0.4, 1.0, CV_THRESH_BINARY);
#ifdef _DEBUG
        imshow("Thresholded and normalized distance transformed image", distanceTransformedImage);
        waitKey();
#endif

        /// Dilate a bit the dist image
        Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
        dilate(distanceTransformedImage, distanceTransformedImage, kernel1);
#ifdef _DEBUG
        imshow("Peaks", distanceTransformedImage);
        waitKey();
#endif

        ///Find markers for watershed algorithm
        Mat dist_8u;
        distanceTransformedImage.convertTo(dist_8u, CV_8U);
        vector<vector<Point>> contours;
        findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        int objectsCount = static_cast<int>(contours.size());

        /// Create the marker image for the watershed algorithm
        Mat markers = Mat::zeros(distanceTransformedImage.size(), CV_32SC1);
        for (int i = 0; i < objectsCount; i++)
        {
            drawContours(markers, contours, i, Scalar::all(i + 1), -1);
        }
        //Marker of background
        circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
#ifdef _DEBUG
        imshow("Markers", markers * 10000);
        waitKey();
#endif

        //Performing watershed algorithm
        watershed(sourceImage, markers);
#ifdef _DEBUG
        Mat mark = Mat::zeros(markers.size(), CV_8UC1);
        markers.convertTo(mark, CV_8UC1);
        bitwise_not(mark, mark);
        imshow("Markers_v2", mark);
        waitKey();
#endif

        //Get objects contours
        vector<ObjectInfo> objectsInfo(objectsCount);

        for (int j = 0; j < markers.cols; j++)
        {
            for (int i = 0; i < markers.rows; i++)
            {
                int index = markers.at<int>(i, j);

                index--;
                if (index >= 0 && index < objectsCount)
                {
                    if (objectsInfo[index].sourceImageRect.y > i ||
                        UNDEFINED == objectsInfo[index].sourceImageRect.y)
                    {
                        objectsInfo[index].sourceImageRect.y = i;
                    }

                    if (objectsInfo[index].sourceImageRect.x > j ||
                        UNDEFINED == objectsInfo[index].sourceImageRect.x)
                    {
                        objectsInfo[index].sourceImageRect.x = j;
                    }

                    if ((objectsInfo[index].sourceImageRect.y + objectsInfo[index].sourceImageRect.height) < i)
                    {
                        objectsInfo[index].sourceImageRect.height = i - objectsInfo[index].sourceImageRect.y;
                    }

                    if ((objectsInfo[index].sourceImageRect.x + objectsInfo[index].sourceImageRect.width) < j)
                    {
                        objectsInfo[index].sourceImageRect.width = j - objectsInfo[index].sourceImageRect.x;
                    }
                }
            }
        }

        //Remove objects with undefined contours
        for (int i = 0; i < objectsInfo.size();)
        {
            if (UNDEFINED == objectsInfo[i].sourceImageRect.x)
            {
                objectsInfo.erase(objectsInfo.begin() + i);
            }
            else
            {
                i++;
            }
        }

        //Get objects images
        vector<cv::Vec3b> colors;
        for (int i = 0; i < objectsInfo.size(); i++)
        {
            cv::Rect& r = objectsInfo[i].sourceImageRect;

            objectsInfo[i].image.create(r.width, r.height, image.type());
            image(r).copyTo(objectsInfo[i].image);
            colors.push_back(GetRandomColor());
        }

        // Fill labeled objects with random colors
        Mat dst;
        image.copyTo(dst);
        for (int i = 0; i < markers.rows; i++)
        {
            for (int j = 0; j < markers.cols; j++)
            {
                int index = markers.at<int>(i, j);
                if (index > 0 && index < static_cast<int>(contours.size()))
                {
                    dst.at<Vec3b>(i, j) = colors[index - 1];
                }
                else
                {
                    dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
                }
            }
        }

#ifdef _DEBUG
        imshow("dst", dst);
        waitKey();
#endif

        return objectsInfo;
    }

    map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>> FindTemplateMatchingValueMap(const ObjectsList& objects);

    map<ObjectsList::const_iterator, double> FindTemplateMatchingValueMapFor(const ObjectInfo& obj, const ObjectsList& objects);

    map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>> FindMostMatchingTemplateMap(const ObjectsList& objects);

    ObjectIteratorList RemoveObjectsGroup(map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>>& maxValMap);

    ObjectIteratorList RemoveObjectsGroup(map<ObjectIterator, double>&);

    ObjectIteratorList RemoveObjectsGroup(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>&);

    vector<ObjectIteratorList> FindObjectsGroups(const ObjectsList& objects)
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

    /*vector<ObjectIteratorList> FindObjectsGroups(const ObjectsList& objects)
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
    }*/

    map<ObjectIterator, double> FindObjectsClassificationParamMap(map<ObjectsList::const_iterator, map<ObjectsList::const_iterator, double>>& templateMatchingValMap);

    /*vector<ObjectIteratorList> FindObjectsGroups(const ObjectsList& objects)
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
    }*/

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

    ObjectIteratorList RemoveObjectsGroup(map<ObjectsList::const_iterator, pair<ObjectsList::const_iterator, double>>& m)
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
