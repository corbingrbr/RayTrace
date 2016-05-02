#include "Plane.h"

#include "Object.h"
#include "Tools.h"
#include "Finish.h"
#include "Pigment.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Plane::Plane(Vector3f normal, float distance, shared_ptr<Pigment> pigment, shared_ptr<Finish> finish) 
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
    cout << "-- Plane --" << endl;
    Tools::printVec3("Normal", -normal);
    Tools::printFloat("Distance", -distance);
    Tools::printPigment(pigment);
    Tools::printFinish(finish);
}
    
