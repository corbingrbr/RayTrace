#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

#include <Eigen/Dense>

class Sphere : public Object {

public:
    
    Sphere(Eigen::Vector3f position, float radius, Eigen::Vector3f pigment, Eigen::Vector4f finish);
    virtual ~Sphere();
    float intersection(Eigen::Vector3f e, Eigen::Vector3f d);
    Eigen::Vector3f getNormal(Eigen::Vector3f hitPoint);
    void print();

private:

    float radius;

};

#endif
