#ifndef _PRINT_H_
#define _PRINT_H_

#include <Eigen/Dense>

class PrintOut {

public:
    
    PrintOut(bool hit, bool isPrimary, Eigen::Vector3f location, Eigen::Vector3f ray, float t, Eigen::Vector3f ambient, Eigen::Vector3f diffuse, Eigen::Vector3f specular);
    PrintOut(bool hit, bool isPrimary, Eigen::Vector3f location, Eigen::Vector3f ray);
    virtual ~PrintOut();
    void print();
    
private:
    
    bool hit;
    bool isPrimary;
    Eigen::Vector3f location;
    Eigen::Vector3f ray;
    float t;
    Eigen::Vector3f ambient;
    Eigen::Vector3f diffuse;
    Eigen::Vector3f specular;
    
};

#endif
