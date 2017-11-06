#include "stdafx.h"
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
        mat.copyTo(typesImage);
        mat.copyTo(objectsImage);
    }

    ObjectsList SegmentObjects(const Mat& sourceImage, bool useLaplacianSharpening);

    map<ObjectsList::const_iterator, vector<ObjectsList::const_iterator>> FindTemplatesObjects(const ObjectsList& objects);

    ImageProcessResult ProcessImage(const cv::Mat& img)
    {
        ImageProcessResult res(img);
        res.detectedObjects = SegmentObjects(img, true);

        for (int i = 0; i < res.detectedObjects.size(); i++)
        {
            rectangle(res.objectsImage, res.detectedObjects[i].sourceImageRect, GetRandomColor(), 3);
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

    void SaveObjectImages(const ObjectsList& objects)
    {
        vector<int> compression_params;
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        for (int i = 0; i < objects.size(); i++)
        {
            imwrite(objects[i].name, objects[i].image, compression_params);
        }
    }

    ObjectsList SegmentObjects(const Mat& sourceImage, bool useLaplacianSharpening)
    {
        Mat image;
        sourceImage.copyTo(image);

        if (useLaplacianSharpening)
        {
            ///Create a kernel that we will use for accuting/sharpening our image
            Mat kernel = (Mat_<float>(3, 3) <<
                1, 1, 1,
                1, -8, 1,
                1, 1, 1
                );
            Mat imgLaplacian;
            Mat sharp = image;
            filter2D(sharp, imgLaplacian, CV_32F, kernel);
            image.convertTo(sharp, CV_32F);
            Mat imgResult = sharp - imgLaplacian;

            imgResult.convertTo(imgResult, CV_8UC3);
            image = imgResult;

#ifdef _DEBUG
            imshow("After laplacian", image);
            waitKey();
#endif
        }

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
            objectsInfo[i].image = Mat(image, objectsInfo[i].sourceImageRect);
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

    map<ObjectsList::const_iterator, vector<ObjectsList::const_iterator>> FindTemplatesObjects(const ObjectsList& objects)
    {
        map<ObjectsList::const_iterator, vector<ObjectsList::const_iterator>> objectsTypesLists;
        vector<ObjectsList::const_iterator> notTypedObjects;

        for (auto pCurrentObj = objects.begin(); objects.end() != pCurrentObj; pCurrentObj++)
        {
            notTypedObjects.push_back(pCurrentObj);
        }

        for (auto pTemplateObj = objects.begin(); notTypedObjects.size() != 0 && objects.end() != pTemplateObj; pTemplateObj++)
        {
            int i = 0;

            while (notTypedObjects.size() > 0 && i < notTypedObjects.size())
            {
                auto pCurrentObj = notTypedObjects[i];

                Mat objImage(pCurrentObj->image);

                //Resize if needed
                if (objImage.rows < pTemplateObj->image.rows)
                {
                    double f = (double)pTemplateObj->image.rows / objImage.rows;
                    resize(objImage, objImage, Size(), f, f);
                }

                if (objImage.cols < pTemplateObj->image.cols)
                {
                    double f = (double)pTemplateObj->image.cols / objImage.cols;
                    resize(objImage, objImage, Size(), f, f);
                }

                //
                Mat result;
                double minVal, maxVal;
                Point minLoc, maxLoc;

                matchTemplate(objImage, pTemplateObj->image, result, TM_CCOEFF_NORMED);
                minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

                if (maxVal > 0.75)
                {
                    objectsTypesLists[pTemplateObj].push_back(
                        pCurrentObj
                    );
                    notTypedObjects.erase(notTypedObjects.begin() + i);
                }
                else
                {
                    i++;
                }
            }
        }

        return objectsTypesLists;
    }

    map<ObjectsList::const_iterator, ObjectsList> FindTemplatesObjects(const ObjectsList& objects, const Mat& image)
    {
        Mat imageCopy;
        image.copyTo(imageCopy);
        int countTypedObjects = 0;
        map<ObjectsList::const_iterator, ObjectsList> objectsTypesLists;

        for (auto pTemplateObj = objects.begin(); objects.end() != pTemplateObj; pTemplateObj++)
        {
            Mat result;
            double minVal, maxVal;
            Point minLoc, maxLoc;

            bool stopSearch = false;
            while (!stopSearch)
            {
                matchTemplate(imageCopy, pTemplateObj->image, result, TM_CCOEFF_NORMED);
                minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

                if (maxVal > 0.8)
                {
                    ObjectInfo objInfo;
                    objInfo.sourceImageRect = Rect(
                        maxLoc,
                        Point(maxLoc.x + pTemplateObj->image.cols, maxLoc.y + pTemplateObj->image.rows)
                    );

                    objectsTypesLists[pTemplateObj].push_back(
                        objInfo
                    );
                }
                else
                {
                    stopSearch = true;
                }
            };
        }

        return objectsTypesLists;
    }

}