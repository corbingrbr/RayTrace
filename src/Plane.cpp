#include "Plane.h"

#include "Object.h"
#include "Tools.h"
#include "Finish.h"
#include "Pigment.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Plane::Plane(Vector3f normal, float distance, shared_ptr<Pigment> pigment, shared_ptr<Finish> finish, Matrix4f invXForm) 
    : Object(pigment, finish, invXForm),
      normal(normal),
      distance(distance)
{
    this->position = normal*distance;
}

Plane::~Plane()
{
}

float Plane::intersection(const Vector3f& e, const Vector3f& d)
{
    // assuming vectors are all normalized
    Vector3f p0 = normal * distance;
    float denom = normal.dot(d);

    Vector3f p0e = p0 - e;
    float t = p0e.dot(normal) / denom;
    
    return t;
}

Vector3f Plane::getNormal(const Vector3f& hitPoint) 
{
    return normal;
}

void Plane::print()
{
    cout << "-- Plane --" << endl;
    Tools::printVec3("Normal", normal);
    Tools::printFloat("Distance", distance);
    Tools::printPigment(pigment);
    Tools::printFinish(finish);
}

int Plane::getID()
{
    return Object::PLANE;
}
    
void Plane::calcBounds(Vector3f *min, Vector3f *max)
{
    // Planes never need their bounds calculated
}
