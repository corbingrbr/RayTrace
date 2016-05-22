#include "BoundingBox.cpp"

#include <Eigen/Dense>

using namespace Eigen;

BoundingBox::BoundingBox(shared_ptr<Object> object) 
    : object(object)
{
    // Get vertices
}

BoundingBox::BoundingBox(Vector3f& min, Vector3f& max)
    : object(NULL),
      min(min),
      max(max)
{
}

BoundingBox::~BoundingBox()
{
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

bool  BoundingBox::intersection(Vector3f& pos, Vector3f& ray)
{
    // Box intersection test
}
