#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

#include <Eigen/Dense>
#include <memory>

class Pigment;
class Finish;

class Sphere : public Object {

public:
    
    Sphere(Eigen::Vector3f position, float radius, std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish);
    virtual ~Sphere();
    float intersection(const Eigen::Vector3f& e, const Eigen::Vector3f& d);
    Eigen::Vector3f getNormal(const Eigen::Vector3f& hitPoint);
    void print();

private:

    float radius;

};

#endif
