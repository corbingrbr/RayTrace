#include "Light.h"

#include <Eigen/Dense>
#include <iostream>

using namespace Eigen;
using namespace std;

Light::Light(Vector3f position, Vector3f color)
    : position(position),
      color(color)
{
}

Light::~Light()
{
}
  
void Light::print()
{
    cout << "-- Light --" << endl;
    cout << "Location: <" << position(0) << ", " << position(1) << ", " << position(2) << ">" << endl;
    cout << "Color: <" << color(0) << ", " << color(1) << ", " << color(2) << ">" << endl;
}
  
Vector3f Light::getPosition()
{
    return position;
}

Vector3f Light::getColor()
{
    return color;
}
    
