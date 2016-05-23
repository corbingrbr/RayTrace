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

Sphere::Sphere(Vector3f position, float radius, shared_ptr<Pigment> pigment, shared_ptr<Finish> finish, Matrix4f invXForm) 
    : Object(pigment, finish, invXForm),
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

int Sphere::getID()
{
    return Object::SPHERE;
}

void Sphere::calcBounds(Vector3f *min, Vector3f *max)
{
    Vector3f p = position;
    float r = radius;
    
    Matrix4f xform = invXForm.inverse();
    
    vector<Vector4f> vertices;
    vertices.push_back(xform * Vector4f(p(0)+r,p(1)+r, p(2)+r, 1)); // 
    vertices.push_back(xform * Vector4f(p(0)+r,p(1)+r, p(2)-r, 1)); // Top portion of bounding box
    vertices.push_back(xform * Vector4f(p(0)-r,p(1)+r, p(2)+r, 1)); //
    vertices.push_back(xform * Vector4f(p(0)-r,p(1)+r, p(2)-r, 1)); //

    vertices.push_back(xform * Vector4f(p(0)+r,p(1)-r, p(2)+r, 1)); // 
    vertices.push_back(xform * Vector4f(p(0)+r,p(1)-r, p(2)-r, 1)); // Bottom portion of bounding box
    vertices.push_back(xform * Vector4f(p(0)-r,p(1)-r, p(2)+r, 1)); //
    vertices.push_back(xform * Vector4f(p(0)-r,p(1)-r, p(2)-r, 1)); //

    float minx, miny, minz, maxx, maxy, maxz;

    calcMinMax(vertices, &minx, &miny, &minz, &maxx, &maxy, &maxz);

    *min = Vector3f(minx, miny, minz);
    *max = Vector3f(maxx, maxy, maxz);
}

void Sphere::calcMinMax(vector<Vector4f> verts, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz)
{
    if (verts.size() > 0) {
        
        float _minx, _miny, _minz, _maxx, _maxy, _maxz;

        _minx = _maxx = verts[0](X);
        _miny = _maxy = verts[0](Y);
        _minz = _maxz = verts[0](Z);
        
        for (unsigned int i = 1; i < verts.size(); i++) {
            _minx = verts[i](X) < _minx ? verts[i](X) : _minx;
            _miny = verts[i](Y) < _miny ? verts[i](Y) : _miny;
            _minz = verts[i](Z) < _minz ? verts[i](Z) : _minz;
            _maxx = verts[i](X) > _maxx ? verts[i](X) : _maxx;
            _maxy = verts[i](Y) > _maxy ? verts[i](Y) : _maxy;
            _maxz = verts[i](Z) > _maxz ? verts[i](Z) : _maxz;
        }
        
        *minx = _minx;
        *miny = _miny;
        *minz = _minz;
        *maxx = _maxx;
        *maxy = _maxy;
        *maxz = _maxz;
    }
}
