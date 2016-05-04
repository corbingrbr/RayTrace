#include "Shade.h"

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

Shade::Shade() :
    ambient(Vector3f(0,0,0)),
    diffuse(Vector3f(0,0,0)),
    specular(Vector3f(0,0,0)),
    color(Vector3f(0,0,0)) 
{
}

Shade::Shade(Vector3f ambient, Vector3f diffuse, Vector3f specular) :
    ambient(ambient),
    diffuse(diffuse),
    specular(specular)
{
    color = ambient + diffuse + specular;
}

Shade::~Shade() 
{
}

Vector3f Shade::getColor()
{
    return color;
}

void Shade::setColor(Vector3f c)
{
    color = c;
}

Vector3f Shade::getAmbient()
{
    return ambient;
}

void Shade::setAmbient(Vector3f a)
{
    ambient = a;
}

Vector3f Shade::getDiffuse()
{
    return diffuse;
}

void Shade::setDiffuse(Vector3f d)
{
    diffuse = d;
}

Vector3f Shade::getSpecular()
{
    return specular;
}

void Shade::setSpecular(Vector3f s)
{
    specular = s;
}

Shade Shade::operator+(Shade& other) {
    Eigen::Vector3f _ambient = ambient + other.getAmbient();
    Eigen::Vector3f _diffuse = diffuse + other.getDiffuse();
    Eigen::Vector3f _specular = specular + other.getSpecular();
    
    return Shade(_ambient, _diffuse, _specular);
}

Shade& Shade::operator*=(float scalar) {
    ambient *= scalar;
    diffuse *= scalar;
    specular *= scalar;
    color *= scalar;

    return *this;
}
