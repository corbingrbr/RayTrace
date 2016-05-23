#include "BoundingBox.h"

#include <Eigen/Dense>
#include <memory>

using namespace Eigen;
using namespace std;

BoundingBox::BoundingBox(shared_ptr<Object> object) 
    : object(object)
{
}

BoundingBox::BoundingBox(Vector3f min, Vector3f max)
    : object(NULL),
      min(min),
      max(max)
{
    center = (min + max) / 2;
}

BoundingBox::~BoundingBox()
{
}

shared_ptr<Object> BoundingBox::getObject()
{
    return object;
}

// Set up bounds of box based on geometry to enwrap
void BoundingBox::setup()
{
    // calc min, max
    if (object != NULL) {
        object->calcBounds(&min, &max);
    }

    center = (min + max) / 2;
}

Vector3f& BoundingBox::getMin()
{
    return min;
}

Vector3f& BoundingBox::getMax()
{
    return max;
}

Vector3f& BoundingBox::getCenter()
{
    return center;
}

bool BoundingBox::intersection(const Vector3f& pos, const Vector3f& ray)
{
    float tmax, tmin = 0;

    for (int i = 0; i < 3; i++) {
 
        float invD = 1.0f / ray(i);
        float t0 = (min(i) - pos(i)) * invD;
        float t1 = (max(i) - pos(i)) * invD;
        if (invD < 0.0f) { swap(&t0, &t1); }
        
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1;;
        if (tmax <= tmin) { return false; }
    }
    
    return true;
} 

void BoundingBox::swap(float *a, float *b) {
    float temp = *a;
    
    *a = *b;
    *b = temp;
}

