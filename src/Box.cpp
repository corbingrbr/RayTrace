#include "Box.h"
#include "Object.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <limits>
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
      
    float tgmin = -numeric_limits<float>::infinity();
    float tgmax = numeric_limits<float>::infinity();

   for (int i = 0; i < 3; i++) {
      float tmin = (min(i) - e(i)) / d(i);
      float tmax = (max(i) - e(i)) / d(i);

      if (tmin > tmax) {
         std::swap(tmin, tmax);
      }

      tgmin = tmin > tgmin ? tmin : tgmin;
      tgmax = tmax < tgmax ? tmax : tgmax;

      if (tgmin > tgmax) {
         return -1.0;
      }
   }

   if (tgmin > tgmax) {
      std::cout << "oops" << std::endl;
   }

   if (tgmin < 0) {
      if (tgmax > 0) {
         return tgmax;
      }
      
      return tgmax;
   }
   return tgmin;   
}


Vector3f Box::getNormal(const Vector3f& hitPoint)
{
    Vector4f hp = invXForm * Vector4f(hitPoint(X), hitPoint(Y), hitPoint(Z), 1);
    
    float epsilon = .0001;
   
    Vector4f normal;
    
    if (hp(X) > min(X) - epsilon && hp(X) <= min(X)) { 
        normal = Vector4f(-1, 0, 0, 0);
    } else if (hp(X) < max(X) + epsilon && hp(X) >= max(X)) { 
        normal = Vector4f(1, 0, 0, 0); 
    } else if (hp(Y) > min(Y) - epsilon && hp(Y) <= min(Y)) { 
        normal = Vector4f(0, 1, 0, 0); 
    } else if (hp(Y) < max(Y) + epsilon && hp(Y) >= max(Y)) {
        normal = Vector4f(0, -1, 0, 0); 
    } else if (hp(Z) > min(Z) - epsilon && hp(Z) <= min(Z)) { 
        normal = Vector4f(0, 0, -1, 0);
    } else { 
        normal = Vector4f(0, 0, 1, 0); 
    }
    
    normal = invXForm.transpose() * normal;
    normal.normalize();
   
    return normal.head(3);
}

void Box::print()
{
    cout << "-- Box --" << endl;
    Tools::printVec3("Location", center);
    Tools::printVec3("Min", min);
    Tools::printVec3("Max", max);
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
