#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Object.h"

#include <memory>
#include <Eigen/Dense>

class Pigment;
class Finish;

class Triangle : public Object {

public:
    
    Triangle(Eigen::Vector3f vertices[], std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish);
    virtual ~Triangle();
    float intersection(Eigen::Vector3f e, Eigen::Vector3f d);
    Eigen::Vector3f getNormal(Eigen::Vector3f hitPoint);
    void init();
    void print();
    
private:

    void calcNormalNPosition();

    enum Vertices { A, B, C };

    enum Dimensions { X, Y, Z };

    Eigen::Vector3f *vertices;
    Eigen::Vector3f normal;

};

#endif
 
