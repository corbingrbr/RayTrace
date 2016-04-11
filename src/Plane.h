#ifndef _PLANE_H_
#define _PLANE_H_

#include "Object.h"

class Plane : public Object {

public:
    Plane(Eigen::Vector3f _normal, float _distance, Eigen::Vector3f _color);
    virtual ~Plane();
    float intersection(Eigen::Vector3f e, Eigen::Vector3f d);
    void print();
    
private:
    
    Eigen::Vector3f normal;
    float distance;

};

#endif
