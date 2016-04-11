#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <Eigen/Dense>
#include <vector>
#include <string>

class Window 
{
    
public:
    
    Window(int _width, int _height);
    virtual ~Window();
    int getWidth();
    int getHeight();
    void setPixel(int i, int j, Eigen::Vector3f color);
    void genImage(std::string outfile);

private:
    
    int width;
    int height;

    std::vector<Eigen::Vector3f> buffer;
};

#endif
