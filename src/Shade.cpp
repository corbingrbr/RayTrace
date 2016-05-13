#include "Shade.h"

#include <Eigen/Dense>
#include <algorithm>
#include "Tools.h"

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

void Shade::clamp()
{
    Tools::clampVec(&ambient, 1.0);
    Tools::clampVec(&diffuse, 1.0);
    Tools::clampVec(&specular, 1.0);
    Tools::clampVec(&color, 1.0);
}

bool Shade::isBlack()
{
    return color(0) == 0 && color(1) == 0 && color(2) == 0;
}

Shade Shade::operator+(Shade& other) 
{
    Vector3f _ambient = ambient + other.getAmbient();
    Vector3f _diffuse = diffuse + other.getDiffuse();
    Vector3f _specular = specular + other.getSpecular();
    
    return Shade(_ambient, _diffuse, _specular);
}

Shade Shade::operator*(float scalar)
{
    Vector3f _ambient = ambient * scalar;
    Vector3f _diffuse = diffuse * scalar;
    Vector3f _specular = specular * scalar;

    return Shade(_ambient, _diffuse, _specular);
}

Shade Shade::operator^(float scalar)
{
    Vector3f _diffuse = diffuse * scalar;
    Vector3f _specular = specular * scalar;

    return Shade(ambient, _diffuse, _specular);
}

Shade& Shade::operator*=(float scalar) 
{
    ambient *= scalar;
    diffuse *= scalar;
    specular *= scalar;
    color *= scalar;

    return *this;
}

Shade& Shade::operator^=(float scalar)
{
    diffuse *= scalar;
    specular *= scalar;

    return *this;
}

