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
        cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);
        cv::flip(image,image,0);
        cv::imwrite(filename, image);
        //cv::imshow("SNAP",image);
    }
}

void VideoFileWriter::WriteImageFile(const std::string& filename,int width, int height, void* bitmapData)
{
    if (bitmapData != nullptr)
    {
        cv::Mat image(cv::Size(width, height),
                      CV_8UC3,
                      (void*)(bitmapData),
                      cv::Mat::AUTO_STEP
                      );
        cv::flip(image,image,0);
        cv::imwrite(filename, image);
        //cv::imshow("SNAP",image);
    }
}


VideoFileWriter::VideoFileWriter(const std::string &filename, double frameRate, long width, long height)
{
    fw.open(filename,cv::VideoWriter::fourcc('a','v','c','1'),frameRate,cv::Size(width,height));
}

VideoFileWriter::~VideoFileWriter()
{
    fw.release();
}

bool VideoFileWriter::isOpened()
{
    return fw.isOpened();
}

void VideoFileWriter::writeBitmapFrame(void *bitmapFrame)
{
    BitmapHeader *bitmapHeader = nullptr;
    if (fw.isOpened() && bitmapFrame != nullptr)
    {
        //std::cout<<"write frame OK"<<std::endl;
        bitmapHeader = reinterpret_cast<BitmapHeader*>(bitmapFrame);
        cv::Mat image(cv::Size(bitmapHeader->width, bitmapHeader->height),
                      CV_8UC4,
                      (void*)(bitmapHeader+1),
                      cv::Mat::AUTO_STEP
                      );
        cv::cvtColor(image, image, cv::COLOR_RGBA2RGB);
        cv::flip(image,image,0);
        fw<<image;
    }
}

