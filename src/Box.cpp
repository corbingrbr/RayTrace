#include "Box.h"
#include "Object.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Box::Box(Vector3f min, Vector3f max, shared_ptr<Pigment> pigment, shared_ptr<Finish> finish, Matrix4f invXForm)
    : Object(pigment, finish, invXForm),
      min(min),
      max(max)
{
    center = (min + max) / 2;
}

Box::~Box()
{
}

float Box::intersection(const Vector3f& e, const Vector3f& d)
{
    float tmax, tmin = 0;

    for (int i = 0; i < 3; i++) {
 
        float invD = 1.0f / d(i);
        float t0 = (min(i) - e(i)) * invD;
        float t1 = (max(i) - e(i)) * invD;
        if (invD < 0.0f) { swap(&t0, &t1); }
        
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1;
        if (tmax <= tmin) { return false; }
    }
    
    return true;
}

Vector3f Box::getNormal(const Vector3f& hitPoint)
{
    /*Vector4f hp = invXForm * Vector4f(hitPoint(X), hitPoint(Y), hitPoint(Z), 1);
    float epsilon = .0001;

    if (hp(X) > min(X) - epsilon && hp(X) <= min(X)) { return Vector3f(-1, 0, 0); }
    if (hp(X) < max(X) + epsilon && hp(X) >= max(X)) { return Vector3f(1, 0, 0); }
    
    if (hp(Y) > min(Y) - epsilon && hp(Y) <= min(Y)) { return Vector3f(0, 1, 0); }
    if (hp(Y) < max(Y) + epsilon && hp(Y) >= max(Y)) { return Vector3f(0, -1, 0); }
    
    if (hp(Z) > min(Z) - epsilon && hp(Z) <= min(Z)) { return Vector3f(0, 0, -1); }
    if (hp(Z) < max(Z) + epsilon && hp(Z) >= max(Z)) { return Vector3f(0, 0, 1); }*/
    
    return Vector3f(-1, 0, 0);
}

void Box::print()
{
    cout << "-- Box --" << endl;
    Tools::printVec3("Location", center);
    Tools::printPigment(pigment);
    Tools::printFinish(finish);
}

int Box::getID()
{
   return Object::BOX;
}

void Box::calcBounds(Vector3f *_min, Vector3f *_max)
{
    Matrix4f xform = invXForm.inverse();
    
    vector<Vector4f> vertices;
    vertices.push_back(xform * Vector4f(min(X),min(Y), min(Z), 1)); // 
    vertices.push_back(xform * Vector4f(min(X),min(Y), max(Z), 1)); // 
    vertices.push_back(xform * Vector4f(min(X),max(Y), min(Z), 1)); // 
    vertices.push_back(xform * Vector4f(min(X),max(Y), max(Z), 1)); // 
    
    vertices.push_back(xform * Vector4f(max(X),min(Y), min(Z), 1)); // 
    vertices.push_back(xform * Vector4f(max(X),min(Y), min(Z), 1)); // 
    vertices.push_back(xform * Vector4f(max(X),max(Y), min(Z), 1)); // 
    vertices.push_back(xform * Vector4f(max(X),max(Y), min(Z), 1)); // 
   

    float minx, miny, minz, maxx, maxy, maxz;

    calcMinMax(vertices, &minx, &miny, &minz, &maxx, &maxy, &maxz);

    *_min = Vector3f(minx, miny, minz);
    *_max = Vector3f(maxx, maxy, maxz);
}

void Box::calcMinMax(vector<Vector4f> verts, float *minx, float *miny, float *minz, float *maxx, float *maxy, float *maxz)
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

void Box::swap(float *a, float *b) {
    float temp = *a;
    
    *a = *b;
    *b = temp;
}
