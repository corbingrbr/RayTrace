#include "Window.h"
#include <string>
#include <cstring>

using namespace Eigen;
using namespace std;

Window::Window(int _width, int _height) :
    width(_width),
    height(_height) {
    
    buffer.assign(width*height, Vector3f(0,0,0));

    }

Window::~Window()
{
}

int Window::getWidth()
{
    return width;
}

int Window::getHeight()
{
    return height;
}

void Window::setPixel(int i, int j, Vector3f color)
{
    buffer[width*j + i] = color;
}

void Window::genImage(string outfile)
{
    FILE *fp = fopen(outfile.c_str(), "w");
    
    if (fp == NULL)
    {
        perror("ERROR: Image::WriteTga() failed to open file for writing!\n");
        exit(EXIT_FAILURE);
    }
    
    // write 24-bit uncompressed targa header
    // thanks to Paul Bourke (http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/)
    putc(0, fp);
    putc(0, fp);
    
    putc(2, fp); // type is uncompressed RGB
    
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    putc(0, fp);
    
    putc(0, fp); // x origin, low byte
    putc(0, fp); // x origin, high byte
    
    putc(0, fp); // y origin, low byte
    putc(0, fp); // y origin, high byte

    putc(width & 0xff, fp); // width, low byte
    putc((width & 0xff00) >> 8, fp); // width, high byte

    putc(height & 0xff, fp); // height, low byte
    putc((height & 0xff00) >> 8, fp); // height, high byte

    putc(24, fp); // 24-bit color depth

    putc(0, fp);

    // write the raw pixel data in groups of 3 bytes (BGR order)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int ndx = x + y*width;

            // if color scaling is on, scale 0.0 -> _max as a 0 -> 255 unsigned byte
            unsigned char rbyte, gbyte, bbyte;

            rbyte = (unsigned char)(buffer[ndx](0) * 255);
            gbyte = (unsigned char)(buffer[ndx](1) * 255);
            bbyte = (unsigned char)(buffer[ndx](2) * 255);

           
            putc(bbyte, fp);
            putc(gbyte, fp);
            putc(rbyte, fp);
        }
    }

    fclose(fp);
}
