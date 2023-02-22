#include <cmath>
#include "Shapes.h"

cv::Scalar Colors[] = { {0,0,255,0},{0,255,0,0},{255,0,0,0} };

Shape::Shape()
    :selector(RED)
{
}

void Shape::setColor(ColorSelector color)
{
    selector = color;
}

Line::Line(int x1, int y1, int x2, int y2):
    start(x1, y1), end(x2, y2)
{
}

void Line::draw(cv::Mat& image)
{
    cv::line(image,start,end,Colors[selector],1);
}

Circle::Circle(int x, int y, int r):
    center(x,y),radius(r)
{
}

void Circle::draw(cv::Mat& image)
{
    cv::circle(image, center, radius, Colors[selector], 1);
}

Grid::Grid(int width, int height, int interval)
{
    reset(width,height,interval);
}

void Grid::draw(cv::Mat& image)
{
    for (auto item : lines) 
    {
        item.setColor(selector);
        item.draw(image);
    }
}

void Grid::reset(int width, int height, int interval)
{
    lines.clear();
    if (width <= 0 || height <= 0 || interval <= 0) return;
    
    // horizontal line
    int pos = height/2;
    while(pos < height)
    {
        lines.emplace_back(0, pos, width, pos);
        pos += interval;
    }
    pos = height/2-1;
    while(pos >= 0)
    {
        lines.emplace_back(0,pos,width,pos);
        pos -= interval;
    }
    // vertical line
    pos = width/2;
    while(pos  <width)
    {
        lines.emplace_back(pos,0,pos,height);
        pos += interval;
    }
    pos = width/2-1;
    while(pos >= 0)
    {
        lines.emplace_back(pos,0,pos,height);
        pos -= interval;
    }
}

CrossHair::CrossHair(int width, int height, int x, int y)
{
    reset(width,height,x,y);
}

void CrossHair::draw(cv::Mat& image)
{
    for (auto item : lines) 
    {
        item.setColor(selector);
        item.draw(image);
    }
}

void CrossHair::reset(int width, int height, int x, int y)
{
    lines.clear();
    if (width <= 0 || height <= 0 || x < 0 || y < 0) return;
    
    lines.emplace_back(0,y,width,y);
    lines.emplace_back(x,0,x,height);
}

Trajectory::Trajectory(int width, int height)
{
    reset(width, height);
}
    
void Trajectory::draw(cv::Mat& image)
{
    cv::Point prePoint;
    bool bFirst = true;
    for (auto& item : pointSequence)
    {
        if (bFirst) 
        {
            bFirst = false;
            prePoint = item;
            continue;
        }
        cv::line(image,prePoint,item,Colors[selector],1);
        prePoint = item;
    }
}

void Trajectory::reset(int w, int h)
{
    pointSequence.clear();
    width = w;
    height = h;
}

void Trajectory::append(int x, int y)// window view pos
{
    int adjustY = height - y;// image view pos
    if (!pointSequence.empty())
    {
        if (std::abs(pointSequence.back().x - x) > 1 || std::abs(pointSequence.back().y - adjustY) > 1)
        {
            pointSequence.emplace_back(x, adjustY);
        }
    }
    else pointSequence.emplace_back(x, adjustY);
}


ShapeCollection ShapeCollection::shapeCollection;

ShapeCollection::ShapeCollection():
    grid(0,0,0),crossHair(0,0,0,0)
{
    grid.setColor(BLUE);
    crossHair.setColor(GREEN);
}

void ShapeCollection::draw(cv::Mat& image)
{
    grid.draw(image);
    crossHair.draw(image);
    for (auto item : commonList)
    {
        item->draw(image);
    }
}

void ShapeCollection::append(Shape* oneShape)
{
    commonList.emplace_back(oneShape);
}

void ShapeCollection::back()
{
    if (!commonList.empty()) commonList.pop_back();
}

void ShapeCollection::reset()
{
    crossHair.reset(0,0,0,0);
    commonList.clear();
}

void ShapeCollection::resetAll()
{
    grid.reset(0,0,0);
    crossHair.reset(0,0,0,0);
    commonList.clear();
}

