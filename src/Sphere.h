#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

#include <Eigen/Dense>
#include <memory>

class Pigment;
class Finish;

class Sphere : public Object {

public:
    
    Sphere(Eigen::Vector3f position, float radius, std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish, Eigen::Matrix4f invXForm);
    virtual ~Sphere();
    float intersection(const Eigen::Vector3f& e, const Eigen::Vector3f& d);
    Eigen::Vector3f getNormal(const Eigen::Vector3f& hitPoint);
    void print();
    int getID();
    void calcBounds(Eigen::Vector3f *min, Eigen::Vector3f *max);
    void calcMinMax(std::vector<Eigen::Vector4f> verts, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz);

private:

    float radius;

};

#endif
