#include "Plane.h"

#include "Object.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Plane::Plane(Vector3f _p, float _d, Vector3f _color) 
    : Object(_color),
      p(_p),
      d(_d) {}

Plane::~Plane()
{
}

float Plane::intersection(Vector3f e, Vector3f d)
{
    return -1.0;
}

void Plane::print()
{
    cout << "<" << p(0) << ", " << p(1) << ", " << p(2) << ">" << endl;
    cout << d << endl;
    cout << "<" << color(0) << ", " << color(1) << ", " << color(2) << ">" << endl;
    
}
    
