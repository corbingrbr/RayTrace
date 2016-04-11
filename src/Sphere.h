#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

#include <Eigen/Dense>

class Sphere : public Object {

public:
    
    Sphere(Eigen::Vector3f _position, float _radius, Eigen::Vector3f _color);
    virtual ~Sphere();
    float intersection(Eigen::Vector3f e, Eigen::Vector3f d);
    void print();

private:

    Eigen::Vector3f position;
    float radius;

};

#endif
