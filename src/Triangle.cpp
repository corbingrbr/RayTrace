#include "Triangle.h"

#include "Tools.h" 
#include "Pigment.h"
#include "Finish.h"

#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

Triangle::Triangle(Vector3f vertices[], shared_ptr<Pigment> pigment, shared_ptr<Finish> finish)
    : Object(pigment, finish), 
      vertices(vertices)
{
}

Triangle::~Triangle()
{
}

float Triangle::intersection(Vector3f p0, Vector3f ray)
{
    /*      A       x       b
        | a d g | | B |   | j |
        | b e h | | y | = | k |
        | c f i | | t |   | l |  */

    float beta, gamma, t = -1.0; // beta, gamma, t -> variables to solve for
    float a, b, c, d, e, f, g, h, i; // components of A
    float j, k, l; // components of b
    float ei_hf, gf_di, dh_eg, ak_jb, jc_al, bl_kc; // Caching of computations


    // A
    a = vertices[A](X) - vertices[B](X); // xa - xb    
    b = vertices[A](Y) - vertices[B](Y); // ya - yb    
    c = vertices[A](Z) - vertices[B](Z); // za - zb
    d = vertices[A](X) - vertices[C](X); // xa - xc
    e = vertices[A](Y) - vertices[C](Y); // ya - yc
    f = vertices[A](Z) - vertices[C](Z); // za - zc
    g = ray(X); // xd
    h = ray(Y); // yd
    i = ray(Z); // zd
    
    j = vertices[A](X) - p0(X); // xa - xe
    k = vertices[A](Y) - p0(Y); // ya - ye
    l = vertices[A](Z) - p0(Z); // za - ze
    

    ei_hf = e*i - h*f;
    gf_di = g*f - d*i;
    dh_eg = d*h - e*g;
    ak_jb = a*k - j*b;
    jc_al = j*c - a*l;
    bl_kc = b*l - k*c;

    float M = a*ei_hf + b*gf_di + c*dh_eg; // determinant of A
  
    // t
    t = (f*ak_jb + e*jc_al + d*bl_kc) / M;
    
    if (t < 0.0f) { return -1.0f; }

    gamma = (i*ak_jb + h*jc_al + g*bl_kc) / M;

    if (gamma < 0 || gamma > 1) { return -1.0f; } 

    beta = (j*ei_hf + k*gf_di + l*dh_eg) / M;
    
    if (beta < 0 || beta > 1 - gamma) { return -1.0f; }
 
    return t; // SUCCESS
}

Vector3f Triangle::getNormal(Vector3f hitPoint)
{
    return normal;
}

void Triangle::init()
{
    calcNormalNPosition();
}

void Triangle::print()
{
    cout << "-- Triangle --" << endl;
    cout << "Vertices: " << endl;
    Tools::printVec3("A", vertices[A]);
    Tools::printVec3("B", vertices[B]);
    Tools::printVec3("C", vertices[C]);
    Tools::printPigment(pigment);
    Tools::printFinish(finish);
}

void Triangle::calcNormalNPosition()
{
    Vector3f v1 = vertices[B] - vertices[A];
    Vector3f v2 = vertices[C] - vertices[A];
    
    normal = (v1.cross(v2)).normalized();
    position = vertices[A] + 0.5*v1 + 0.5*v2;
}
