#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <Eigen/Dense>
#include <memory>
#include "Pigment.h"
#include "Finish.h"

class Object {

public:

Object(std::shared_ptr<Pigment> pigment, std::shared_ptr<Finish> finish) 
    : pigment(pigment),
        finish(finish) 
        {
        }
    
    virtual float intersection(Eigen::Vector3f p0, Eigen::Vector3f d) = 0;
    virtual Eigen::Vector3f getNormal(Eigen::Vector3f hitPoint) = 0;
    virtual void print() = 0;
    std::shared_ptr<Pigment> getPigment() { return pigment; };
    std::shared_ptr<Finish> getFinish() { return finish; }
    Eigen::Vector3f getPosition() { return position; }

    Eigen:: Vector3f getRGB() { return pigment->getRGB(); }
    float getFilter() { return pigment->getFilter(); }
    float getAmbient() { return finish->getAmbient(); }
    float getDiffuse() { return finish->getDiffuse();}
    float getSpecular() { return finish->getSpecular(); }
    float getRoughness() { return finish->getRoughness(); }
    float getReflection() { return finish->getReflection(); }
    float getRefraction() { return finish->getRefraction(); }
    float getIOR() { return finish->getIOR(); }
    bool isReflective() { return finish->getReflection() > 0.0f; }
    bool isRefractive() { return finish->getRefraction() > 0.0f; }

protected:
    
    Eigen::Vector3f position;
    std::shared_ptr<Pigment> pigment;
    std::shared_ptr<Finish> finish;

};

#endif
