#ifndef _SHAPES_H_
#define _SHAPES_H_
#include <opencv2/opencv.hpp>
#include <vector>

enum ColorSelector
{
    RED = 0,
    GREEN,
    BLUE      
};

class Shape
{
public:
    Shape();
    
    virtual void draw(cv::Mat& image) = 0;
    virtual void setColor(ColorSelector color);
protected:
    ColorSelector selector;
};

class Line : public Shape
{
public:
    Line(int x1, int y1, int x2, int y2);

    void draw(cv::Mat& image) override;
private:
    cv::Point start;
    cv::Point end;
};

class Circle : public Shape
{
public:
    Circle(int x, int y, int r);

    void draw(cv::Mat& image) override;
private:
    cv::Point center;
    int radius;
};

class Grid : public Shape
{
public:
    Grid(int width, int height, int interval);

    void draw(cv::Mat& image) override;
    //void setColor(ColorSelector color) override;
    void reset(int width, int height, int interval);
private:
    std::vector<Line> lines;
};

class CrossHair : public Shape
{
public:
    CrossHair(int width, int height, int x, int y);

    void draw(cv::Mat& image) override;
    //void setColor(ColorSelector color) override;
    void reset(int width, int height, int x, int y);
private:
    std::vector<Line> lines;
};

class Trajectory : public Shape
{
public:
    Trajectory(int width, int height);
    
    void draw(cv::Mat& image) override;

    void reset(int width, int height);
    void append(int x, int y);
private:
    int width;
    int height;
    std::vector<cv::Point> pointSequence;
};

class ShapeCollection
{
public:
    ShapeCollection();

    void draw(cv::Mat& image);
    void resetGrid(int width, int height, int interval){grid.reset(width,height,interval);}
    void resetCrossHair(int width, int height, int x, int y){crossHair.reset(width,height,x,height-y);}
    void setCrossHairColor(ColorSelector color){crossHair.setColor(color);}
    void append(Shape* oneShape);
    void back();
    void reset();
    void resetAll();

    static ShapeCollection& get(){return shapeCollection;}

private:
    static ShapeCollection shapeCollection;
    
    Grid grid;
    CrossHair crossHair;
    std::vector< std::shared_ptr<Shape> > commonList;
};

#endif
