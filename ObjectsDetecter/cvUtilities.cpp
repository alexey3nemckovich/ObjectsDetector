#include "stdafx.h"
#include "cvUtilities.h"


namespace cvutils
{

    ImageProcessResult::ImageProcessResult(const cv::Mat& mat)
    {
        mat.copyTo(groupsImage);
        mat.copyTo(objectsImage);
    }

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

    ObjectsList SegmentObjects(const Mat& sourceImage);

    ImageProcessResult ProcessImage(cv::Mat& img, ObjectsClassifyingAlgorithm classifyingAlgorithm)
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
        auto& objGroups = res.detectedObjectsGroups = std::move(classifyingAlgorithm(res.detectedObjects));
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
                        ObjectInfo::UNDEFINED_COORDINATE == objectsInfo[index].sourceImageRect.y)
                    {
                        objectsInfo[index].sourceImageRect.y = i;
                    }

                    if (objectsInfo[index].sourceImageRect.x > j ||
                        ObjectInfo::UNDEFINED_COORDINATE == objectsInfo[index].sourceImageRect.x)
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
            if (ObjectInfo::UNDEFINED_COORDINATE == objectsInfo[i].sourceImageRect.x)
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

}
