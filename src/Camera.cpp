#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "Object.h"
#include "Light.h"
#include "Tools.h"
#include "Pigment.h"
#include "Finish.h"
#include "Shade.h"
#include "PrintOut.h"

#include <memory>
#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <stack>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

#define UNIT_TEST true
#define HIT true
#define REFL_RECURSES 5
#define AIR 1.0

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
    Vector3f ray;
    Shade shade;

    float completed = 0;
    float inc = 100.00 / (window->getWidth() * window->getHeight());

    cout << setprecision(1) << fixed;
    cout << "Casting rays ..." << flush;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            
            // Generate ray direction from camera into scene
            ray = calcRay(i, j, window);

            // Cast ray to get pixel color 
            shade = castRay(NULL, location, ray, scene, !UNIT_TEST, REFL_RECURSES, NULL);

            // Set color of pixel
            window->setPixel(i, j, shade.getColor()); 

            completed += inc;
        }
    }

    cout << " Done" << endl;
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running unit tests ..." << endl << endl;

    unitTest(320, 145, window, scene);
    cout << endl << endl << endl;
    
    unitTest(315, 185, window, scene);

    cout << endl << endl << endl;
    unitTest(220, 240, window, scene);

}

void Camera::unitTest(int i, int j, shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    cout << "Pixel: [" << i << ", " << j << "]" << endl;
    
    shared_ptr<stack<PrintOut> > log = make_shared<stack<PrintOut> >();
    
    Vector3f ray = calcRay(i, j, window);

    Shade s = castRay(NULL, location, ray, scene, UNIT_TEST, REFL_RECURSES, log);

    Tools::printVec3("Color", s.getColor());
    cout << "------------" << endl << endl;

    while (!log->empty()) {
        log->top().print();
        log->pop();
    }
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

Shade Camera::castRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene, bool unitTest, int iteration, shared_ptr<stack<PrintOut> > log) {

    Shade localColor = Shade();
    Shade reflectColor = Shade();
    Shade refractColor = Shade();

    // Fire ray into scene to detect objects
    pair<float, shared_ptr<Object> > hitObject = intersectRay(avoid, loc, ray, scene);
    shared_ptr<Object> object = hitObject.second; // object which is intersected by ray
    float t = hitObject.first; // intersection is t-distance along ray

    // Check whether an object is hit
    if (object != NULL) {
       
        float reflect = object->getReflection();
        float filter = object->getFilter();

        // Determine point in space which ray intersects object
        Vector3f hitPoint = loc + t * ray;

        // Calculate Local Color
        localColor = calcLocal(scene, object, hitPoint);

        if (object->isReflective() && iteration != 0) {
            reflectColor = calcReflection(scene, object, iteration, hitPoint, ray, unitTest, log);
        }

        // Calculate Refraction color       
        if (object->isRefractive()) {
            refractColor = calcRefraction(scene, object, hitPoint, ray, unitTest);
        }
        
        reflectColor *= reflect;
        refractColor *= filter;
        localColor = localColor ^ (1 - reflect - filter); // ^ operator -> only scales diffuse & specular
        Shade color = localColor + reflectColor + refractColor;
       
        color.clamp();
        
        // Unit test print out
        if (unitTest) {
            bool isPrimary = iteration == REFL_RECURSES ? true : false;
            
            PrintOut p = PrintOut(HIT, isPrimary, loc, ray, t, color.getAmbient(), color.getDiffuse(), color.getSpecular());

            log->push(p); 
        }
        
        return color;
    }


    if (unitTest) {

        bool isPrimary = iteration == REFL_RECURSES ? true : false;        
        PrintOut p = PrintOut(!HIT, isPrimary, loc, ray);      
        log->push(p);
    }
    
    return Shade();
}

bool Camera::isShadowed(shared_ptr<Scene> scene, shared_ptr<Light> light, shared_ptr<Object> avoid, const Vector3f& hitPoint)
{
    // Ray from hit point to light in scene
    Vector3f hit2Light = light->getPosition() - hitPoint;
    Vector3f feeler = hit2Light.normalized();
    
    // Cast shadow feeler ray  ---- TODO --- ERROR: it collides with 'avoid' ptr sometimes
    pair<float, shared_ptr<Object> > shadowObject = intersectRay(avoid, hitPoint, feeler, scene);

    return shadowObject.second != NULL &&  (shadowObject.first * feeler).norm() < hit2Light.norm();
}

Shade Camera::calcLocal(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& hitPoint)
{
    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);

    // Iterate through lights in scene
    shared_ptr<Light> light = scene->getLight();

    if (!isShadowed(scene, light, object, hitPoint)) {
    
        // Light vector
        Vector3f hit2Light = light->getPosition() - hitPoint;
        Vector3f feeler = hit2Light.normalized();
        Vector3f normal = object->getNormal(hitPoint);
   
        diffuse = calcDiffuse(object, normal, feeler, light);
        specular = calcSpecular(object, hitPoint, normal, feeler, light);
    }

    ambient = calcAmbient(object);
    
    // Local Color
    Shade s = Shade(ambient, diffuse, specular);
    s.clamp();

    return s; 
}

Shade Camera::calcReflection(shared_ptr<Scene> scene, shared_ptr<Object> object, int iteration, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest, shared_ptr<stack<PrintOut> > log)
{       
    Vector3f normal = object->getNormal(hitPoint);
    Vector3f reflRay = ray - 2*(ray.dot(normal))*normal;
    reflRay.normalize();

    Shade reflectColor = castRay(object, hitPoint, reflRay, scene, unitTest, iteration - 1, log);
        
    return reflectColor; 
}

Shade Camera::calcRefraction(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest)
{
    float n1, n2;
    bool entering;
    float ior = object->getIOR();

    Vector3f normal = object->getNormal(hitPoint);

    // Check whether ray is leaving or entering object
    entering = ray.dot(normal) < 0 ? true : false;
    
    n1 = entering ? AIR : ior;
    n2 = entering ? ior : AIR;


    if (!entering) {
        normal = -normal;
    }
        
    Vector3f refractRay = calcRefractionRay(n1, n2, ray, normal);
    
    if (unitTest) {
        cout << "-- Refraction --" << endl;
        Tools::printVec3("Location", hitPoint);
        Tools::printVec3("RefractRay", refractRay);
        cout << endl << endl;
    }

    // Account for internal reflection
    if (refractRay(0) == 0 && refractRay(1) == 0 && refractRay(2) == 0) {
      
        return Shade();
    
    } else {
        // Minute adjustment to position to mitigate collision errors
        Vector3f newLoc = hitPoint + refractRay * .0001; 
        
        if (entering) {
            
            pair<float, shared_ptr<Object> > p = intersectRay(NULL, newLoc, refractRay, scene);
            Vector3f hitAt = newLoc + refractRay * p.first;
            return calcRefraction(scene, object, hitAt, refractRay, unitTest); 
       
        } else {
           
            return castRay(NULL, newLoc, refractRay, scene, !UNIT_TEST, REFL_RECURSES, NULL);
        }
    }
}

pair<float, shared_ptr<Object> > Camera::intersectRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene)
{
    // ERROR here where avoid is actually not avoided ... take warning
    return scene->intersections(avoid, loc, ray);
}

Vector3f Camera::calcRefractionRay(float n1, float n2, const Vector3f& ray, const Vector3f& normal)
{
    float radicand = sqrt(1 - pow(n1 / n2, 2) * (1 - pow(ray.dot(normal), 2)));
    
    // Account for total internal reflection
    return radicand < 0 ? Vector3f(0,0,0) : (n1 / n2) * (ray - normal * ray.dot(normal)) - normal * radicand; 
}

Vector3f Camera::calcAmbient(shared_ptr<Object> object)
{
    return object->getRGB() * object->getAmbient();
}

Vector3f Camera::calcDiffuse(shared_ptr<Object> object, const Vector3f& normal, const Vector3f& feeler, shared_ptr<Light> light)
{
    // Dot product the two rays to determine diffused light coefficient
    float d = max(normal.dot(feeler), 0.0f);
    return Tools::mult2Vecs(object->getRGB() * object->getDiffuse() * d, light->getColor());
}

Vector3f Camera::calcSpecular(shared_ptr<Object> object, const Vector3f& hitPoint, const Vector3f& normal, const Vector3f& feeler, shared_ptr<Light> light)
{
    
    Vector3f view = (location - hitPoint).normalized();
    Vector3f half = (view + feeler) / (view + feeler).norm();

    return Tools::mult2Vecs(object->getRGB() * object->getSpecular() * pow(normal.dot(half), 1 / object->getRoughness()), light->getColor());
}

