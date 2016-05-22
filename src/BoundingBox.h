#ifndef _BBOX_H_
#define _BBOX_H_

#include "Object.h"

#include <Eigen/Dense>
#include <memory>

class BoundingBox {
    
public:
    BoundingBox(std::shared_ptr<Object> object);
    BoundingBox(Eigen::Vector3f min, Eigen::Vector3f max);
    virtual ~BoundingBox();
    void setup();
    std::shared_ptr<Object> getObject();
    Eigen::Vector3f& getMin();
    Eigen::Vector3f& getMax();
    Eigen::Vector3f& getCenter();
    bool intersection(const Eigen::Vector3f& pos, const Eigen::Vector3f& ray);
       
private:
    Eigen::Vector3f min;
    Eigen::Vector3f max;
    Eigen::Vector3f center;
  
    std::shared_ptr<Object> object;
};

#endif
