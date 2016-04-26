#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Eigen/Dense>

class Light {
    
public:
    
    Light(Eigen::Vector3f position, Eigen::Vector3f color);
    virtual ~Light();
    
    void print();
    Eigen::Vector3f getPosition();
    Eigen::Vector3f getColor();
    
private:

    Eigen::Vector3f position;
    Eigen::Vector3f color;

};

#endif
