#ifndef _PLANE_H_
#define _PLANE_H_

#include <memory>

#include "Object.h"

class Pigment;
class Finish;

class Plane : public Object {

public:
    Plane(Eigen::Vector3f normal, float distance, std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish, Eigen::Matrix4f invXForm);
    virtual ~Plane();
    float intersection(const Eigen::Vector3f& e, const Eigen::Vector3f& d);
    Eigen::Vector3f getNormal(const Eigen::Vector3f& hitPoint);
    void print();
    
private:
    
    Eigen::Vector3f normal;
    float distance;

};

#endif
