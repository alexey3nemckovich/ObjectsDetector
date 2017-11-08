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

}
