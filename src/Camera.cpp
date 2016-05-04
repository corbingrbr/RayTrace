#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "Object.h"
#include "Light.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"
#include "Shade.h"

#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

#define UNIT_TEST true
#define REFL_RECURSES 5

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
    Shade shade;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            
            // Generate ray direction from camera into scene
            ray = calcRay(i, j, window);
            
            // Cast ray to get pixel color 
            shade = castRay(location, ray, scene, !UNIT_TEST, REFL_RECURSES);
            
            // Set color of pixel
            window->setPixel(i, j, shade.getColor());
            
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
    castRay(location, calcRay(i, j, window), scene, UNIT_TEST, REFL_RECURSES);
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

Shade Camera::castRay(Vector3f location, Vector3f ray, shared_ptr<Scene> scene, bool unitTest, int iteration) {

    Shade localColor = Shade();
    Shade reflectColor = Shade();
    Shade refractColor = Shade();
    float reflect = 0.0f; // Reflection
    float filter = 0.0f;  // Refraction


    // Fire ray into scene to detect objects
    pair<float, shared_ptr<Object> > hitObject = intersectRay(ray, scene);
    shared_ptr<Object> object = hitObject.second; // object which is intersected by ray
    float t = hitObject.first; // intersection is t-distance along ray

    // Check whether an object is hit
    if (object != NULL) {

        // Determine point in space which ray intersects object
        Vector3f hitPoint = location + t * ray;

        // Calculate Local Color
        localColor = calcLocalColor(scene, object, hitPoint); 

        // Calculate Reflection Color
        if (object->isReflective() && iteration != 0) {

            reflect = object->getReflection();
            Vector3f normal = object->getNormal(hitPoint);
            Vector3f reflRay = ray + 2*((-ray).dot(normal))*normal;
            reflectColor = castRay(hitPoint, reflRay, scene, !UNIT_TEST, iteration - 1);
            reflectColor *= reflect;
        } 

        if (object->isRefractive()) {
            // Do stuff
            refractColor *= object->getFilter();
        }

        Shade color = localColor + reflectColor + refractColor;

        // Unit test print out
        if (unitTest) {
            cout << "Iteration type: " << (iteration == REFL_RECURSES ? "Primary" : "Reflection") << endl;
            
            Tools::printVec3("Location", location);
            Tools::printVec3("Ray", ray);
            Tools::printFloat("T", hitObject.first);
            Tools::printVec3("Ambient", color.getAmbient());
            Tools::printVec3("Diffuse", color.getDiffuse());
            Tools::printVec3("Specular", color.getSpecular());
            
            cout << endl << endl;
        }
        
        return color;
    }


    if (unitTest) {
        cout << "Iteration type: " << (iteration == REFL_RECURSES ? "Primary" : "Reflection") << endl;
        Tools::printVec3("Location", location);
        Tools::printVec3("Ray", ray);
        cout << "No hit" << endl; 
    }
    
    return Shade();
}

Shade Camera::calcLocalColor(shared_ptr<Scene> scene, shared_ptr<Object> object, Vector3f hitPoint)
{
    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);
    float reflect = object->getReflection();
    float filter = object->getFilter();

    // Grab light
    shared_ptr<Light> light = scene->getLight();
    
    // Ray from hit point to light in scene
    Vector3f hit2Light = light->getPosition() - hitPoint;
    Vector3f feeler = hit2Light.normalized();
    
    // Cast shadow feeler ray
    pair<float, shared_ptr<Object> > shadowObject = castShadowRay(object, hitPoint, feeler, scene);
    float tShadow = shadowObject.first;
    
    // Calc ray from center of object to hit point
    Vector3f normal = object->getNormal(hitPoint);
    
    // If shadow cast on pixel then color ambient only
    if (shadowObject.second == NULL || (tShadow * feeler).norm() > hit2Light.norm()) {
        
        diffuse = (1 - reflect - filter) * calcDiffuse(object, normal, feeler, light);
        specular = (1 - reflect - filter) * calcSpecular(object, hitPoint, normal, feeler, light);
    }
    
    ambient = calcAmbient(object);

    // Local Color
    return Shade(ambient, diffuse, specular); 
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

Vector3f Camera::calcDiffuse(shared_ptr<Object> object, Vector3f normal, Vector3f feeler, shared_ptr<Light> light)
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

