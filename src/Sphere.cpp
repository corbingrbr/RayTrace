#include "Sphere.h"
#include "Object.h"

#include <iostream>
#include <algorithm>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Sphere::Sphere(Vector3f _position, float _radius, Vector3f _color) 
    : Object(_color),
      position(_position), 
      radius(_radius) {}
    
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

void Sphere::print()
{
    cout << "<" << position(0) << ", " << position(1) << ", " << position(2) << ">" << endl;
    cout << radius << endl;
    cout << "<" << color(0) << ", " << color(1) << ", " << color(2) << ">" << endl;
}
