#include "Plane.h"

#include "Object.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Plane::Plane(Vector3f _normal, float _distance, Vector3f _color) 
    : Object(_color),
      normal(_normal),
      distance(_distance) {}

Plane::~Plane()
{
}

float Plane::intersection(Vector3f e, Vector3f d)
{
    // assuming vectors are all normalized
    float denom = normal.dot(d);
    Vector3f p0 = normal * distance;

    if (denom > 1e-6) {
        Vector3f p0e = p0 - e;
        float t = p0e.dot(normal) / denom;

        return t;
    }

    return -1.0f; 
}

void Plane::print()
{
    cout << "<" << normal(0) << ", " << normal(1) << ", " << normal(2) << ">" << endl;
    cout << distance << endl;
    cout << "<" << color(0) << ", " << color(1) << ", " << color(2) << ">" << endl;
    
}
    
