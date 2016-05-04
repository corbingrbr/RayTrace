#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <Eigen/Dense>
#include <memory>

class Pigment;
class Finish;

class Tools {

public:
    
    static Eigen::Vector3f mult2Vecs(Eigen::Vector3f a, Eigen::Vector3f b);
    static void printVec3(std::string name, Eigen::Vector3f v);
    static void printVec4(std::string name, Eigen::Vector4f v);
    static void printFloat(std::string name, float f); 
    static void printPigment(std::shared_ptr<Pigment> p);
    static void printFinish(std::shared_ptr<Finish> f);
    static Eigen::Vector3f clampVec(Eigen::Vector3f v, float f);

private:

};

#endif 
