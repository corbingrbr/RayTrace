#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Object.h"

#include <memory>
#include <Eigen/Dense>

class Pigment;
class Finish;

class Triangle : public Object {

public:
    
    Triangle(Eigen::Vector3f vertices[], std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish, Eigen::Matrix4f invXForm);
    virtual ~Triangle();
    float intersection(const Eigen::Vector3f& e, const Eigen::Vector3f& d);
    Eigen::Vector3f getNormal(const Eigen::Vector3f& hitPoint);
    void init();
    void print();
    int getID();
    void calcBounds(Eigen::Vector3f *min, Eigen::Vector3f *max);

private:
    void calcMinMax(std::vector<Eigen::Vector4f> verts, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz);

    float getMinBound(int axis);
    float getMaxBound(int axis);
    void calcNormalNPosition();

    enum Vertices { A, B, C };

    Eigen::Vector3f *vertices;
    Eigen::Vector3f normal;

};

#endif
 
