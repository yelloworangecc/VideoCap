#ifndef _VIDEO_FILE_WRITER_H
#define _VIDEO_FILE_WRITER_H

#include <string>
#include <opencv2/opencv.hpp>

struct BitmapHeader
{
    unsigned int size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int sizeImage;
    int xPelsPerMeter;
    int yPelsPerMeter;
    unsigned int clrUsed;
    unsigned int clrImportant;
};

class VideoFileWriter
{
public:
    static void WriteImageFile(const std::string& filename, void* bitmapFrame);
    static void WriteImageFile(const std::string& filename,int width, int height, void* bitmapData);
    VideoFileWriter(const std::string& filename, double frameRate, long videoWidth, long videoHeight);
    ~VideoFileWriter();
    bool isOpened();
    void writeBitmapFrame(void* bitmapFrame);

private:
    cv::VideoWriter fw;
};

#endif
