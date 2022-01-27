#include <iostream>
#include "VideoFileWriter.h"

void VideoFileWriter::WriteImageFile(const std::string& filename, void* bitmapFrame)
{
    BitmapHeader *bitmapHeader = nullptr;
    if (bitmapFrame != nullptr)
    {
        bitmapHeader = reinterpret_cast<BitmapHeader*>(bitmapFrame);
        cv::Mat image(cv::Size(bitmapHeader->width, bitmapHeader->height),
                      CV_8UC4,
                      (void*)(bitmapHeader+1),
                      cv::Mat::AUTO_STEP
                      );
        cv::flip(image,image,0);
        cv::imwrite(filename, image);
        cv::imshow("SNAP",image);
    }
}

VideoFileWriter::VideoFileWriter(const std::string &filename,long videoWidth, long videoHeight)
{
    fw.open(filename,cv::VideoWriter::fourcc('M','J','P','G'),15.0,cv::Size(videoWidth,videoHeight));
}

VideoFileWriter::~VideoFileWriter()
{
    fw.release();
}

void VideoFileWriter::writeBitmapFrame(void *bitmapFrame)
{
    BitmapHeader *bitmapHeader = nullptr;
    if (fw.isOpened() && bitmapFrame != nullptr)
    {
        bitmapHeader = reinterpret_cast<BitmapHeader*>(bitmapFrame);
        cv::Mat image(cv::Size(bitmapHeader->width, bitmapHeader->height),
                      CV_8UC4,
                      (void*)(bitmapHeader+1),
                      cv::Mat::AUTO_STEP
                      );
        cv::flip(image,image,0);
        cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);
        fw<<image;
    }
}

