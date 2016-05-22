#include "BoundingBox.h"

#include <Eigen/Dense>
#include <memory>

using namespace Eigen;
using namespace std;

BoundingBox::BoundingBox(shared_ptr<Object> object) 
    : object(object)
{
    // Get vertices
}

BoundingBox::BoundingBox(Vector3f min, Vector3f max)
    : object(NULL),
      min(min),
      max(max)
{
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
    // calc min, max, and center
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

bool  BoundingBox::intersection(const Vector3f& pos, const Vector3f& ray)
{
    // Box intersection test
}
