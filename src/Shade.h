#ifndef _SHADE_H_
#define _SHADE_H_

#include <Eigen/Dense>

class Shade {

public:
    
    Shade& operator*=(float scalar);
    Shade operator+(Shade& other);

    Shade();
    Shade(Eigen::Vector3f ambient, Eigen::Vector3f diffuse, Eigen::Vector3f specular);
    virtual ~Shade();
    Eigen::Vector3f getColor();
    void setColor(Eigen::Vector3f color);
    Eigen::Vector3f getAmbient();
    void setAmbient(Eigen::Vector3f ambient);
    Eigen::Vector3f getDiffuse();
    void setDiffuse(Eigen::Vector3f diffuse);
    Eigen::Vector3f getSpecular();
    void setSpecular(Eigen::Vector3f specular);
    
        
private:

    Eigen::Vector3f color;
    Eigen::Vector3f ambient;
    Eigen::Vector3f diffuse;
    Eigen::Vector3f specular;

};

#endif
