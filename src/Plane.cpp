#include "Plane.h"

#include "Object.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Plane::Plane(Vector3f normal, float distance, Vector3f pigment, Vector4f finish) 
    : Object(pigment, finish),
      normal(normal),
      distance(distance)
{
    this->position = normal*distance;
}

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

Vector3f Plane::getNormal(Vector3f hitPoint) 
{
    return -normal.normalized();
}

void Plane::print()
{
    cout << "<" << normal(0) << ", " << normal(1) << ", " << normal(2) << ">" << endl;
    cout << distance << endl;
    cout << "Pigment: <" << pigment(0) << ", " << pigment(1) << ", " << pigment(2) << ">" << endl;
    cout << "Finish: <" << finish(0) << ", " << finish(1) << ", " << finish(2) << ", " << finish(3) << ">" << endl; 
}
    
