#include "Sphere.h"
#include "Object.h"

#include <iostream>
#include <algorithm>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Sphere::Sphere(Vector3f position, float radius, Vector3f pigment, Vector4f finish) 
    : Object(pigment, finish),
      radius(radius) 
{
    this->position = position;
}
    
Sphere::~Sphere()
{
}

float Sphere::intersection(Vector3f e, Vector3f d)
{
    // Check discriminant first
    float discriminant = sqrt(pow(d.dot(e-position), 2) - d.dot(d) * ((e-position).dot(e-position) - radius* radius));
    
    // Calculate intersection point, make sure t > 0
    if (discriminant < 0.0) {
        
        return -1.0f;
        
    } else if (discriminant == 0.0) {
        
        float t = (-d).dot(e - position) / (d.dot(d));
         
        return t;
        
    } else {
        
        float t1 = ((-d).dot(e - position) + discriminant) / (d.dot(d));
        float t2 = ((-d).dot(e - position) - discriminant) / (d.dot(d));
        
        if (t1 < 0) { return t2; }
        if (t2 < 0) { return t1; }
        
        return min(t1, t2);
    }  
}

Vector3f Sphere::getNormal(Eigen::Vector3f hitPoint)
{
    return (hitPoint - position).normalized();
}

void Sphere::print()
{
    cout << "-- Sphere --" << endl;
    cout << "Location: <" << position(0) << ", " << position(1) << ", " << position(2) << ">" << endl;
    cout << "Radius: " << radius << endl;
    cout << "Pigment: <" << pigment(0) << ", " << pigment(1) << ", " << pigment(2) << ">" << endl;
    cout << "Finish: <" << finish(0) << ", " << finish(1) << ", " << finish(2) << ", " << finish(3) << ">" << endl; 
}
