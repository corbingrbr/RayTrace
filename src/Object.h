#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <Eigen/Dense>

class Object {

public:

Object(Eigen::Vector3f pigment, Eigen::Vector4f finish) :
    pigment(pigment),
        finish(finish) {}
    
    virtual float intersection(Eigen::Vector3f p0, Eigen::Vector3f d) = 0;
    virtual Eigen::Vector3f getNormal(Eigen::Vector3f hitPoint) = 0;
    virtual void print() = 0;
    Eigen::Vector3f getPigment() { return pigment; };
    Eigen::Vector4f getFinish() { return finish; }
    Eigen::Vector3f getPosition() { return position; }

    float getAmbient() { return finish(0); }
    float getDiffuse() { return finish(1); }
    float getSpecular() { return finish(2); }
    float getRoughness() { return finish(3); }

protected:
    
    Eigen::Vector3f position;
    Eigen::Vector3f pigment;
    Eigen::Vector4f finish;

};

#endif
