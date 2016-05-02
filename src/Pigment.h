#ifndef _PIGMENT_H_
#define _PIGMENT_H_

#include <Eigen/Dense>

class Pigment {
    
public:

    Pigment(Eigen::Vector3f rgb, float filter)
        : rgb(rgb),
        filter(filter) {}

    virtual ~Pigment() {};
    Eigen::Vector3f getRGB() { return rgb; }
    float getFilter() { return filter; }

private:
    
    Eigen::Vector3f rgb;
    float filter;
    
};

#endif
