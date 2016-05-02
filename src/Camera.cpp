#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "Object.h"
#include "Light.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"

#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

#define UNIT_TEST true

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
    cout << "Casting rays ... ";
   
    Vector3f ray;
    Vector3f color;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            
            // Generate ray direction from camera into scene
            ray = calcRay(i, j, window);
            
            // Cast ray to get pixel color 
            color = castRay(ray, scene, !UNIT_TEST);
            
            // Set color of pixel
            window->setPixel(i, j, color);
            
        } 
    }

    cout << "Done" << endl;
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running unit tests ..." << endl << endl;

    unitTest(120, 120, window, scene);
    unitTest(295, 265, window, scene);
    unitTest(420, 130, window, scene);
}

void Camera::unitTest(int i, int j, shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    cout << "Pixel: [" << i << ", " << j << "]" << endl;
    castRay(calcRay(i, j, window), scene, UNIT_TEST);
}

void Camera::print()
{
    Tools::printVec3("location", location);
    Tools::printVec3("up", up);
    Tools::printVec3("right", right);
    Tools::printVec3("lookAt", lookAt);
}

Vector3f Camera::calcRay(int i, int j, shared_ptr<Window> window)
{
    // Use window to construct ray vectors
    float rght = right.norm() / 2;
    float left = -rght;

    float top = up.norm() / 2;
    float bottom = -top;

    float us = left + (rght - left)*(i + 0.5)/window->getWidth();
    float vs = bottom + (top - bottom)*(j + 0.5)/window->getHeight();
    
    Vector3f ray = us*right.normalized() + vs*up.normalized() + (lookAt - location).normalized(); 
    
    ray.normalize();
    
    return ray;
}

Vector3f Camera::castRay(Vector3f ray, shared_ptr<Scene> scene, bool unitTest) {
   
    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffused = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);
    Vector3f feeler = Vector3f(0,0,0);

    // Fire ray into scene to detect objects
    pair<float, shared_ptr<Object> > hitObject = intersectRay(ray, scene);
    shared_ptr<Object> object = hitObject.second;
    float t = hitObject.first;

    // Check whether an object is hit
    if (object != NULL) {

        // Determine point in space which ray intersects object
        Vector3f hitPoint = location + t * ray;

        // Grab light
        shared_ptr<Light> light = scene->getLight();
                        
        // Ray from hit point to light in scene
        Vector3f hit2Light = light->getPosition() - hitPoint;
        feeler = hit2Light.normalized();
        

        // Cast shadow feeler ray
        pair<float, shared_ptr<Object> > shadowObject = castShadowRay(object, hitPoint, feeler, scene);
        float tShadow = shadowObject.first;
        

        // If shadow cast on pixel then color ambient only
        if (shadowObject.second == NULL || (tShadow * feeler).norm() > hit2Light.norm()) {
            
            // Calc ray from center of object to hit point
            Vector3f normal = object->getNormal(hitPoint);

            diffused = calcDiffused(object, normal, feeler, light);
         
            specular = calcSpecular(object, hitPoint, normal, feeler, light);
        }
        
        ambient = calcAmbient(object);



        // Calc Reflective Color

    }
    

    if (unitTest) {
        Tools::printVec3("Ray", ray);
        Tools::printFloat("T", hitObject.first);
        Tools::printVec3("Ambient", ambient*255);
        Tools::printVec3("Diffuse", diffused*255);
        Tools::printVec3("Specular", specular*255);
        Tools::printVec3("Feeler", feeler);
        cout << endl << endl;
    }

    Vector3f color = ambient + diffused + specular;

    // Clamp the values 
    if (color(0) > 1.0f) { color(0) = 1.0f; }
    if (color(1) > 1.0f) { color(1) = 1.0f; }
    if (color(2) > 1.0f) { color(2) = 1.0f; }

    return color;
}

pair<float, shared_ptr<Object> > Camera::intersectRay(Vector3f ray, shared_ptr<Scene> scene)
{
    return scene->intersections(NULL, location, ray);
}

pair<float, shared_ptr<Object> > Camera::castShadowRay(shared_ptr<Object> object, Vector3f loc, Vector3f ray, shared_ptr<Scene> scene)
{
    return scene->intersections(object, loc, ray);
}

Vector3f Camera::calcAmbient(shared_ptr<Object> object)
{
    return object->getRGB() * object->getAmbient();
}

Vector3f Camera::calcDiffused(shared_ptr<Object> object, Vector3f normal, Vector3f feeler, shared_ptr<Light> light)
{
    // Dot product the two rays to determine diffused light coefficient
    float d = max(normal.dot(feeler), 0.0f);
    
    // Calculate diffused
    return Tools::mult2Vecs(object->getRGB() * object->getDiffuse() * d, light->getColor());
}

Vector3f Camera::calcSpecular(shared_ptr<Object> object, Vector3f hitPoint, Vector3f normal, Vector3f feeler, shared_ptr<Light> light)
{
    
    Vector3f view = (location - hitPoint).normalized();
    Vector3f half = (view + feeler) / (view + feeler).norm();

    return Tools::mult2Vecs(object->getRGB() * object->getSpecular() * pow(normal.dot(half), 1 / object->getRoughness()), light->getColor());
}

