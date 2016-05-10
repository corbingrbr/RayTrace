#include "Sphere.h"
#include "Object.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"

#include <iostream>
#include <algorithm>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Sphere::Sphere(Vector3f position, float radius, shared_ptr<Pigment> pigment, shared_ptr<Finish> finish) 
    : Object(pigment, finish),
      radius(radius)
{
    this->position = position;
}
    
Sphere::~Sphere()
{
}

float Sphere::intersection(const Vector3f& e, const Vector3f& d)
{
    // Check discriminant first
    float discriminant = sqrt(pow(d.dot(e-position), 2) - d.dot(d) * ((e-position).dot(e-position) - radius * radius));
    
    // Calculate intersection point, make sure t > 0
    if (discriminant < 0.0) {
        
        return -1.0f;
        
    } else if (discriminant == 0.0) {
        
        float t = (-d).dot(e - position) / d.dot(d);
         
        return t;
        
    } else {
        
        float t1 = (-d.dot(e - position) + discriminant) / d.dot(d);
        float t2 = (-d.dot(e - position) - discriminant) / d.dot(d);
        
        if (t1 < 0) { return t2; }
        if (t2 < 0) { return t1; }
        
        return min(t1, t2);
    }  
}

Vector3f Sphere::getNormal(const Vector3f& hitPoint)
{
    return (hitPoint - position).normalized();
}

void Sphere::print()
{
    cout << "-- Sphere --" << endl;
    Tools::printVec3("Location", position);
    Tools::printFloat("Radius", radius);
    Tools::printPigment(pigment);
    Tools::printFinish(finish);
  
}
