#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "Object.h"

#include <memory>
#include <iostream>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

Camera::Camera(Vector3f _location, Vector3f _up, Vector3f _right, Vector3f _lookAt) :
    location(_location),
    up(_up),
    right(_right),
    lookAt(_lookAt)
{
}
    
Camera::~Camera()
{
}

void Camera::castRays(shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    cout << endl << endl;
    cout << "Casting rays ... " << endl;
    
    shared_ptr<Object> object;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            object = castRay(calcRay(i, j, window), scene);
            
            if (object != NULL) {
                // Set pixel color to be that of the object
                window->setPixel(i, j, object->getColor());
            }
            
        }
    }
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running ray tests ..." << endl;
}

Vector3f Camera::calcRay(int i, int j, shared_ptr<Window> window)
{
    // Use window to construct ray vectors
    float left = -right.norm() / 2;
    float rght = right.norm() / 2;
    float top = up.norm() / 2;
    float bottom = -up.norm() / 2;

    float us = left + (rght - left)*(i + 0.5)/window->getWidth();
    float vs = bottom + (top - bottom)*(j + 0.5)/window->getHeight();

    float ws = location(2) - 1.0;

    /*cout << "us: " << us << endl;
    cout << "vs: " << vs << endl;
    cout << "ws: " << ws << endl;*/

    
    Vector3f ray = Vector3f(us, vs, ws) - location; 
    ray.normalize();
    
    return ray;
}

shared_ptr<Object> Camera::castRay(Vector3f ray, shared_ptr<Scene> scene)
{
    return scene->intersections(location, ray);
}

void Camera::print()
{
    cout << "<" << location(0) << ", " << location(1) << ", " << location(2) << ">" << endl;
    cout << "<" << up(0) << ", " << up(1) << ", " << up(2) << ">" << endl;
    cout << "<" << right(0) << ", " << right(1) << ", " << right(2) << ">" << endl;
    cout << "<" << lookAt(0) << ", " << lookAt(1) << ", " << lookAt(2) << ">" << endl;
}
