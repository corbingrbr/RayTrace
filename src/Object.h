#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <Eigen/Dense>

class Object {

public:

Object(Eigen::Vector3f _color) :
    color(_color) {}
    
    virtual float intersection(Eigen::Vector3f p0, Eigen::Vector3f d) = 0;
    virtual void print() = 0;
    Eigen::Vector3f getColor() { return color; };

protected:
    
    Eigen::Vector3f color;

};

#endif
