#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "Object.h"
#include "Light.h"

#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

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
    cout << "Casting rays ... ";
    
    pair<float, shared_ptr<Object> > hitObject;
    shared_ptr<Object> object;
    float t;
    Vector3f ray;

    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);
    Vector3f feeler = Vector3f(0,0,0);

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            
            // Generate ray direction from camera into scene
            ray = calcRay(i, j, window);
            
            // Create Cast Ray Function

            // Fire ray into scene to detect objects
            pair<float, shared_ptr<Object> > hitObject = castRay(ray, scene);
            shared_ptr<Object> object = hitObject.second;
            float t = hitObject.first;
            
            // Check whether an object is hit
            if (object != NULL) {
                
                // Grab color of object
                Vector3f pigment = object->getPigment();
                
                // Determine point in space which ray intersects object
                Vector3f hitPoint = location + t * ray;
                
                // Grab light info
                shared_ptr<Light> light = scene->getLight();
                Vector3f lColor = light->getColor();
                Vector3f lPos = light->getPosition();
                
                // Ray from hit point to light in scene
                Vector3f hit2Light = light->getPosition() - hitPoint;
                feeler = hit2Light.normalized();
                
                
                // Cast shadow feeler ray
                pair<float, shared_ptr<Object> > shadowObject = castShadowRay(hitPoint, hit2Light, scene);
                float tShadow = shadowObject.first;
        
                
                // If shadow cast on pixel then color ambient only
                if (shadowObject.second == NULL || (tShadow * feeler).norm() < hit2Light.norm()) {
                    
                    // Calc ray from center of object to hit point
                    Vector3f normal = object->getNormal(hitPoint);
                    
                    ////////// DIFFUSE //////////
                    
                    // Dot product the two rays to determine diffused light coefficient
                    float d = max(normal.dot(feeler), 0.0f);
                    
                    // Calculate diffused
                    diffuse = mult2Vecs(pigment * object->getDiffuse() * d, lColor);
                    
                    ////////// SPECULAR //////////
                    
                    Vector3f view = (location - hitPoint).normalized();
                    Vector3f half = view + hit2Light / (view + hit2Light).norm();
                    
                    specular = mult2Vecs(pigment * object->getSpecular() * pow(normal.dot(half), object->getRoughness()), lColor);
                    
                } else {
                    cout << "SHADOW" << endl;
                }
                
                ambient = pigment * object->getAmbient();
            }
            
            // Pixel color is combination of ambient and diffuse shading
            window->setPixel(i, j, ambient + diffuse + specular); 
            
        } 
    }

    cout << "Done" << endl;
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running ray tests ..." << endl << endl;

    unitTest(120, 120, window, scene);
    unitTest(295, 265, window, scene);
    unitTest(420, 130, window, scene);
}

void Camera::unitTest(int i, int j, shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    // Generate ray direction from camera into scene
    Vector3f ray = calcRay(i, j, window);

    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);
    Vector3f feeler = Vector3f(0,0,0);

    // Fire ray into scene to detect objects
    pair<float, shared_ptr<Object> > hitObject = castRay(ray, scene);
    shared_ptr<Object> object = hitObject.second;
    float t = hitObject.first;

    // Check whether an object is hit
    if (object != NULL) {

        // Grab color of object
        Vector3f pigment = object->getPigment();

        // Determine point in space which ray intersects object
        Vector3f hitPoint = location + t * ray;

        // Grab light info
        shared_ptr<Light> light = scene->getLight();
        Vector3f lColor = light->getColor();
        Vector3f lPos = light->getPosition();
                       
        // Ray from hit point to light in scene
        Vector3f hit2Light = light->getPosition() - hitPoint;
        feeler = hit2Light.normalized();
        

        // Cast shadow feeler ray
        pair<float, shared_ptr<Object> > shadowObject = castShadowRay(hitPoint, hit2Light, scene);
        float tShadow = shadowObject.first;
        

        // If shadow cast on pixel then color ambient only
        if (shadowObject.second == NULL || (tShadow * feeler).norm() < hit2Light.norm()) {
            
            // Calc ray from center of object to hit point
            Vector3f normal = object->getNormal(hitPoint);

            ////////// DIFFUSE //////////
            
            // Dot product the two rays to determine diffused light coefficient
            float d = max(normal.dot(feeler), 0.0f);

            // Calculate diffused
            diffuse = mult2Vecs(pigment * object->getDiffuse() * d, lColor);
            
            ////////// SPECULAR //////////
            
            Vector3f view = (location - hitPoint).normalized();
            Vector3f half = view + hit2Light / (view + hit2Light).norm();

            specular = mult2Vecs(pigment * object->getSpecular() * pow(normal.dot(half), object->getRoughness()), lColor);

        } else {
            cout << "SHADOW" << endl;
        }
        
        ambient = pigment * object->getAmbient();
    }

    cout << "Pixel: [" << i << ", " << j << "]" << endl;
    printVec("Ray", ray);
    cout << "T: " << hitObject.first << endl;
    printVec("Ambient", ambient*255);
    printVec("Diffuse", diffuse*255);
    printVec("Specular", specular*255);
    printVec("Feeler", feeler);
    cout << endl << endl;
    
}

void Camera::print()
{
    printVec("location", location);
    printVec("up", up);
    printVec("right", right);
    printVec("lookAt", lookAt);
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

pair<float, shared_ptr<Object> > Camera::castRay(Vector3f ray, shared_ptr<Scene> scene)
{
    return scene->intersections(location, ray);
}

pair<float, shared_ptr<Object> > Camera::castShadowRay(Vector3f loc, Vector3f ray, shared_ptr<Scene> scene)
{
    return scene->intersections(loc, ray);
}

Vector3f Camera::mult2Vecs(Vector3f a, Vector3f b)
{
    float x,y,z;

    x = a(0) * b(0);
    y = a(1) * b(1);
    z = a(2) * b(2);
    
    return Vector3f(x,y,z);
}

void Camera::printVec(string name, Vector3f v) 
{
    cout << name << ": <" << v(0) << ", " << v(1) << ", " << v(2) << ">" << endl;
}
