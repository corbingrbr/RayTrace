#ifndef _BOX_H_
#define _BOX_H_

#include "Object.h"

#include <Eigen/Dense>
#include <memory>
#include <vector>

class Pigment;
class Finish;

class Box : public Object {

public:
    
    Box(Eigen::Vector3f min, Eigen::Vector3f max, std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish, Eigen::Matrix4f invXForm);
    virtual ~Box();
    float intersection(const Eigen::Vector3f& e, const Eigen::Vector3f& d);
    Eigen::Vector3f getNormal(const Eigen::Vector3f& hitPoint);
    void print();
    int getID();
    void calcBounds(Eigen::Vector3f *_min, Eigen::Vector3f *_max);

private:
    void calcMinMax(std::vector<Eigen::Vector4f> verts, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz);
    void swap(float *a, float *b);

    Eigen::Vector3f min;
    Eigen::Vector3f max;
    Eigen::Vector3f center;

};

#endif
