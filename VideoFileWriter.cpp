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
        cv::imwrite(filename, image);
    }
}

VideoFileWriter::VideoFileWriter(const std::string &filename)
{
    fw.open(filename,cv::VideoWriter::fourcc('M','J','P','G'),15.0,cv::Size(640,480));
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
        cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);
        fw<<image;
    }
}

