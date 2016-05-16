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
#include <cstdlib>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

#define UNIT_TEST true
#define HIT true
#define REFL_RECURSES 5
#define AIR 1.0
#define EPSILON .00001

Camera::Camera(Vector3f _location, Vector3f _up, Vector3f _right, Vector3f _lookAt, int AA_res, bool jitter) :
    location(_location),
    up(_up),
    right(_right),
    lookAt(_lookAt),
    AA_res(AA_res),
    jitter(jitter)
{
    rght = right.norm() / 2;
    left = -rght;
    top = up.norm() / 2;
    bottom = -top;
}
    
Camera::~Camera()
{
}

void Camera::castRays(shared_ptr<Window> window, shared_ptr<Scene> scene)
{  
    vector<Vector3f> rays;
    Shade shade;

    float completed = 0;
    //float inc = 100.00 / (window->getWidth() * window->getHeight());

    cout << setprecision(1) << fixed;
    cout << "Casting rays ..." << flush;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
            
            Vector3f color = Vector3f(0,0,0);

            // Generate ray direction from camera into scene
            rays = calcRays(i, j, window);

            // Cast ray to get pixel color 
            for (int k = 0; k < rays.size(); k++) {
                shade = castRay(NULL, location, rays[k], scene, !UNIT_TEST, REFL_RECURSES, NULL, PRIMARY);
                color += shade.getColor();
            }
            
            color /= rays.size();

            // Set color of pixel
            window->setPixel(i, j, color); 

            //completed += inc;
        }
    }

    cout << " Done" << endl;
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running unit tests ..." << endl << endl;

    unitTest(325, 240, window, scene);
    unitTest(330, 250, window, scene);
}

void Camera::unitTest(int i, int j, shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    cout << "Pixel: [" << i << ", " << j << "]" << endl;
    
    shared_ptr<stack<PrintOut> > log = make_shared<stack<PrintOut> >();
    
    // Generate ray direction from camera into scene
    vector<Vector3f> rays = calcRays(i, j, window);
    
    Shade s = castRay(NULL, location, rays[0], scene, UNIT_TEST, REFL_RECURSES, log, PRIMARY);

    Tools::printVec3("Color", s.getColor());
    cout << "------------" << endl << endl;

    while (!log->empty()) {
        log->top().print();
        log->pop();
    }
    
    cout << endl << "---------------------------------" << endl << endl << endl;
}

void Camera::print()
{
    Tools::printVec3("location", location);
    Tools::printVec3("up", up);
    Tools::printVec3("right", right);
    Tools::printVec3("lookAt", lookAt);
}

Vector3f Camera::calcRay(float dx, float dy, shared_ptr<Window> window)
{
    // Use window to construct ray vectors
    float us = left + (rght - left) * (dx / window->getWidth());
    float vs = bottom + (top - bottom) * (dy / window->getHeight());
    
    Vector3f ray = us*right.normalized() + vs*up.normalized() + (lookAt - location).normalized(); 
    
    ray.normalize();
    
    return ray;
}

vector<Vector3f> Camera::calcRays(int i, int j, shared_ptr<Window> window)
{
    vector<Vector3f> rays;
    Vector3f ray;
    
    float s = 1.0 / (AA_res + 1);
    float d = 1.0 / AA_res;   
    
    for (int x = 1; x <= AA_res; x++) {
        for (int y = 1; y <= AA_res; y++) {

            float dx = i;
            float dy = j;

            if (jitter) {
                dx += Tools::randRange((x-1) * d, x*d);
                dy += Tools::randRange((y-1) * d, y*d);
            } else {
                dx += x*s;
                dy += y*s;
            }

            ray = calcRay(dx, dy, window);
            rays.push_back(ray);
        }
    }

    return rays;
}

Shade Camera::castRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene, bool unitTest, int iteration, shared_ptr<stack<PrintOut> > log, int type) {

    // Fire ray into scene to detect objects
    pair<float, shared_ptr<Object> > hitObject = intersectRay(avoid, loc, ray, scene);
    shared_ptr<Object> object = hitObject.second; // object which is intersected by ray
    float t = hitObject.first; // intersection is t-distance along ray

    Shade color;

    // Check whether the ray has intersected an object within the scene
    if (object != NULL && iteration > 0) {
       
        float reflect = object->getReflection();
        float filter = object->getFilter();

        // Determine point in space which ray intersects object
        Vector3f hitPoint = loc + t * ray;


        if (!object->isRefractive()) {
           
            if (!object->isReflective()) {
               
                color = calcLocal(scene, object, hitPoint);
                
            } else {
                
                    Shade localColor = calcLocal(scene, object, hitPoint);
                    Shade reflectColor;
                    
                    if (!calcReflection(scene, object, iteration, hitPoint,
                                        ray, unitTest, log, &reflectColor)) {
                        reflect = 0;
                    }
                
                    
                    localColor ^= (1 - reflect);
                    reflectColor *= reflect;
                    color = localColor + reflectColor;
            }
        
        } else {  
            color = calcRefraction(scene, object, iteration, hitPoint, ray, unitTest, log);
        }
     
    } else { 
        color = Shade();   
    }

    color.clamp();

    // Unit Testing
    if (unitTest) {
        
        PrintOut p;
        
        if (object != NULL) {
          p = PrintOut(HIT, type, loc, ray, t, color.getAmbient(), color.getDiffuse(), color.getSpecular());
        } else {
          p = PrintOut(!HIT, type, loc, ray);   
        }
        
        log->push(p);
    }
    
    return color;
}

bool Camera::isShadowed(shared_ptr<Scene> scene, shared_ptr<Light> light, shared_ptr<Object> avoid, const Vector3f& hitPoint)
{
    // Ray from hit point to light in scene
    Vector3f hit2Light = light->getPosition() - hitPoint;
    Vector3f feeler = hit2Light.normalized();
    
    pair<float, shared_ptr<Object> > shadowObject = intersectRay(avoid, hitPoint, feeler, scene);

    return shadowObject.second != NULL && (shadowObject.first * feeler).norm() < hit2Light.norm();
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
        Vector3f n = object->getNormal(hitPoint);
        Vector3f normal = (object->getInvXForm() * Vector4f(n(0), n(1), n(2), 0)).head(3);
   
        diffuse = calcDiffuse(object, normal, feeler, light);
        specular = calcSpecular(object, hitPoint, normal, feeler, light);
    } 
    
    ambient = calcAmbient(object);
    
    // Local Color
    Shade s = Shade(ambient, diffuse, specular);
    s.clamp();

    return s; 
}

bool Camera::calcReflection(shared_ptr<Scene> scene, shared_ptr<Object> object, int iteration, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest, shared_ptr<stack<PrintOut> > log, Shade *color)
{       
    Vector3f normal = object->getNormal(hitPoint);
    Vector3f reflRay = ray - 2*(ray.dot(normal))*normal;
    reflRay.normalize();

    *color = castRay(object, hitPoint, reflRay, scene, unitTest, iteration - 1, log, REFLECT);
    
    return !color->isBlack() ? true : false;
}

Shade Camera::calcRefraction(shared_ptr<Scene> scene, shared_ptr<Object> object, int iteration, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest, shared_ptr<stack<PrintOut> > log)
{
    Shade reflectColor;
    Shade refractColor;

    float c, ior = object->getIOR();

    Vector3f refractRay;
    Vector3f normal = object->getNormal(hitPoint);

    // Check whether ray is leaving or entering object
    bool entering = ray.dot(normal) < 0 ? true : false;
    
    // Index of refraction setting
    float n1 = entering ? AIR : ior;
    float n2 = entering ? ior : AIR;
    
    calcReflection(scene, object, iteration, hitPoint, ray, !UNIT_TEST, NULL, &reflectColor);

    if (entering) {
        
        refractRay = calcRefractionRay(n1, n2, ray, normal);
        c = -ray.dot(normal);
        
    } else {
       
        /*kr = exp(-art);
        kg = exp(-agt);
        kb = exp(-abt);*/
        
        refractRay = calcRefractionRay(n1, n2, ray, -normal); 
        
        if (refractRay(0) > 0 || refractRay(1) > 0 || refractRay(2) > 0) {
            c = refractRay.dot(normal);
        } else {       
            return reflectColor;
        }      
    }
    
    Vector3f newLoc = hitPoint + refractRay * EPSILON;

    refractColor = castRay(NULL, newLoc, refractRay, scene, unitTest, iteration-1, log, REFRACT);
   
    float R0 = pow(ior - 1, 2) / pow(ior + 1, 2);
    float R = R0 + (1 - R0) * pow(1-c, 5);

    refractColor *= (1 - R);
    reflectColor *= R;
    
    Shade color = refractColor + reflectColor;
    color.clamp();

    
    return color;
}

pair<float, shared_ptr<Object> > Camera::intersectRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene)
{
    return scene->intersections(avoid, loc, ray);
}

Vector3f Camera::calcRefractionRay(float n1, float n2, const Vector3f& ray, const Vector3f& normal)
{
    float radicand = sqrt(1 - pow(n1 / n2, 2) * (1 - pow(ray.dot(normal), 2)));
    
    // Returns Vector3f(0,0,0) if total internal reflection
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

