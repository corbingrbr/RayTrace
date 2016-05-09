#include "PrintOut.h"
#include "Tools.h"
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

PrintOut::PrintOut(bool hit, bool isPrimary, Vector3f location, Vector3f ray, float t, Vector3f ambient, Vector3f diffuse, Vector3f specular) 
    : hit(hit), 
      isPrimary(isPrimary),
      location(location),
      ray(ray),
      t(t),
      ambient(ambient),
      diffuse(diffuse),
      specular(specular)
{
}

PrintOut::PrintOut(bool hit, bool isPrimary, Vector3f location, Vector3f ray) 
    : hit(hit), 
      isPrimary(isPrimary),
      location(location),
      ray(ray)
{
}

PrintOut::~PrintOut()
{
}

void PrintOut::print()
{
    cout << "Iteration type: " << (isPrimary ? "Primary" : "Reflection") << endl;

    if (hit) {
        
        Tools::printVec3("Location", location);
        Tools::printVec3("Ray", ray);
        Tools::printFloat("T", t);
        Tools::printVec3("Ambient", ambient);
        Tools::printVec3("Diffuse", diffuse);
        Tools::printVec3("Specular", specular);

    } else {
        
        Tools::printVec3("Location", location);
        Tools::printVec3("Ray", ray);
        cout << "No hit" << endl; 
    }
        
    cout << "------------------" << endl << endl;
}
