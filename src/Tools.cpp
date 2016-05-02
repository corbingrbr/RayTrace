#include "Tools.h"

#include "Pigment.h"
#include "Finish.h"

#include <Eigen/Dense>
#include <iostream>
#include <memory>

using namespace Eigen;
using namespace std;

Vector3f Tools::mult2Vecs(Vector3f a, Vector3f b)
{
    float x,y,z;

    x = a(0) * b(0);
    y = a(1) * b(1);
    z = a(2) * b(2);
    
    return Vector3f(x,y,z);
}

void Tools::printVec3(string name, Vector3f v) 
{
    cout << name << ": <" << v(0) << ", " << v(1) << ", " << v(2) << ">" << endl;
}

void Tools::printVec4(string name, Vector4f v)
{
    cout << name << ": <" << v(0) << ", " << v(1) << ", " << v(2) << "," << v(3) << ">" << endl; 
}

void Tools::printFloat(string name, float f) 
{
    cout << name << ": " << f << endl;
}

void Tools::printPigment(std::shared_ptr<Pigment> p)
{
    Vector3f rgb = p->getRGB();

    float f = p->getFilter();
    
    if (f > 0.0f) {
        printVec4("Pigment", Vector4f(rgb(0), rgb(1), rgb(2), f));
    } else {
        printVec3("Pigment", rgb);
    }
}

void Tools::printFinish(std::shared_ptr<Finish> f)
{
    cout << "-- Finish --" << endl;
    printFloat("Ambient", f->getAmbient()); 
    printFloat("Diffuse", f->getDiffuse()); 
    printFloat("Specular",f->getSpecular()); 
    printFloat("Roughness", f->getRoughness()); 
    printFloat("Reflection", f->getReflection()); 
    printFloat("Refraction", f->getRefraction()); 
    printFloat("IOR", f->getIOR()); 
}
