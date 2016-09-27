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
#include "HitRecord.h"

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
#define IND_LIGHT_BOUNCES 2
#define AIR 1.0
#define EPSILON .00001
#define FIRST_GILL 20
#define SECOND_GILL 10
#define THICKNESS .1

Camera::Camera(Vector3f _location, Vector3f _up, Vector3f _right, Vector3f _lookAt, int AA_res, bool jitter, bool gi, bool cartoon, bool isBlinnPhong) :
    location(_location),
    up(_up),
    right(_right),
    lookAt(_lookAt),
    AA_res(AA_res),
    jitter(jitter),
    gi(gi),
    cartoon(cartoon),
    isBlinnPhong(isBlinnPhong)
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
    int tot_hash = 40;
    int n_hash;
    float inc = (float)tot_hash / window->getWidth();

    cout << "Casting rays ... [" << string(tot_hash, '_') << "]" << flush;

    for (int i = 0; i < window->getWidth(); i++) {
        for (int j = 0; j < window->getHeight(); j++) {
        
            Vector3f color = Vector3f(0,0,0);

            // Generate ray direction from camera into scene
            rays = calcRays(i, j, window);

            // Cast ray to get pixel color 
            for (int k = 0; k < rays.size(); k++) {
                shade = castRay(NULL, location, rays[k], scene, !UNIT_TEST, REFL_RECURSES, NULL, PRIMARY, IND_LIGHT_BOUNCES);
                color += shade.getColor();
            }
            
            color /= rays.size();

            // Set color of pixel
            window->setPixel(i, j, color);
        }
        
        // For progress bar demonstrating completion of render 
        completed += inc;
        n_hash = floor(completed);
        cout << "\rCasting rays ... " << "[" << 
            string(n_hash, '#') << string(tot_hash - n_hash, '_') << "]" << flush;
    }

    cout << "\rCasting rays ... Done" << string(tot_hash, ' ') << endl;
}

void Camera::unitTests(shared_ptr<Window> window, shared_ptr<Scene> scene) 
{
    cout << endl << endl;
    cout << "Running unit tests ..." << endl << endl;


    unitTest(72, 80, window, scene);
}

void Camera::unitTest(int i, int j, shared_ptr<Window> window, shared_ptr<Scene> scene)
{
    cout << "Pixel: [" << i << ", " << j << "]" << endl;
    
    shared_ptr<stack<PrintOut> > log = make_shared<stack<PrintOut> >();
    
    // Generate ray direction from camera into scene
    vector<Vector3f> rays = calcRays(i, j, window);
    
    Shade s = castRay(NULL, location, rays[0], scene, UNIT_TEST, REFL_RECURSES, log, PRIMARY, IND_LIGHT_BOUNCES);

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

Shade Camera::castRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene, bool unitTest, int iteration, shared_ptr<stack<PrintOut> > log, int type, int gillumiter) {

    // Fire ray into scene to detect objects
    HitRecord hitRecord = intersectRay(avoid, loc, ray, scene);
    shared_ptr<Object> object = hitRecord.getObject();
    float t = hitRecord.getT();

    Shade color;

    // Check whether the ray has intersected an object within the scene
    if (object != NULL && iteration > 0) {

        float reflect = object->getReflection();
        float filter = object->getFilter();

        // Determine point in space which ray intersects object
        Vector3f hitPoint = loc + t * ray;
        Vector3f normal = object->getNormal(hitPoint);

        if (!object->isRefractive()) {

            if (!object->isReflective()) {
               
                color = calcLocal(scene, object, ray, hitPoint, gillumiter);
                
            } else {

                Shade localColor = calcLocal(scene, object, ray, hitPoint, gillumiter);
                Shade reflectColor;
                    
                if (!calcReflection(scene, object, iteration, hitPoint,
                                    ray, unitTest, log, &reflectColor, gillumiter)) {
                    reflect = 0;
                }
                
                localColor ^= (1 - reflect);
                reflectColor *= reflect;
                color = localColor + reflectColor;
            }
            
        } else {  

            color = calcRefraction(scene, object, iteration, hitPoint, ray, unitTest, log, gillumiter);

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
    
    HitRecord hitRecord = intersectRay(avoid, hitPoint, feeler, scene);

    return hitRecord.getObject() != NULL && (hitRecord.getT() * feeler).norm() < hit2Light.norm();
}

Shade Camera::calcLocal(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& ray, const Vector3f& hitPoint, int gillumiter)
{   
    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);

    Vector3f normal = object->getNormal(hitPoint);

    // Iterate through lights in scene
    vector<shared_ptr<Light> > lights = scene->getLights();
    
    for (int i = 0; i < lights.size(); i++) {
        
        if (!isShadowed(scene, lights[i], object, hitPoint)) {
            
            // Light vector
            Vector3f hit2Light = lights[i]->getPosition() - hitPoint;
            Vector3f feeler = hit2Light.normalized();
            
            diffuse += calcDiffuse(object, normal, feeler, lights[i]);

            if (isBlinnPhong) {
                specular += calcSpecular(object, hitPoint, normal, feeler, lights[i]);
            } else {
                specular += cookTorrance(object, lights[i], ray, normal);
            }
        } 
    }

    
    if (gi) {
        ambient = calcMonteCarlo(scene, object, normal, hitPoint, gillumiter);
    } else {
        ambient = calcAmbient(object);
    }
    
    Shade s = Shade(ambient, diffuse, specular);
    s.clamp();

    return s; 
}

bool Camera::calcReflection(shared_ptr<Scene> scene, shared_ptr<Object> object, int iteration, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest, shared_ptr<stack<PrintOut> > log, Shade *color, int gillumiter)
{       
    Vector3f normal = object->getNormal(hitPoint);
    Vector3f hp;
    Vector3f n = normal * .000001;
    hp << hitPoint(0) + n(0), hitPoint(1) + n(1), hitPoint(2) + n(2); 
    
    Vector3f reflRay = ray - 2*(ray.dot(normal))*normal;
    reflRay.normalize();

    *color = castRay(object, hp, reflRay, scene, unitTest, iteration - 1, log, REFLECT, gillumiter);
    
    return !color->isBlack() ? true : false;
}

Shade Camera::calcRefraction(shared_ptr<Scene> scene, shared_ptr<Object> object, int iteration, const Vector3f& hitPoint, const Vector3f& ray, bool unitTest, shared_ptr<stack<PrintOut> > log, int gillumiter)
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
    
    calcReflection(scene, object, iteration, hitPoint, ray, !UNIT_TEST, NULL, &reflectColor, gillumiter);

    if (entering) {
        
        refractRay = calcRefractionRay(n1, n2, ray, normal);
        c = -ray.dot(normal);
        
    } else {
        
        refractRay = calcRefractionRay(n1, n2, ray, -normal); 
        
        // Internal Reflection
        if (refractRay(0) == 0 && refractRay(1) == 0 && refractRay(2) == 0) { 
            return reflectColor;
        }    

        c = ray.dot(normal);
    }
    
    Vector3f newLoc = hitPoint + refractRay * EPSILON;

    refractColor = castRay(NULL, newLoc, refractRay, scene, unitTest, iteration-1, log, REFRACT, gillumiter);
   
    /*float R0 = pow((n1 - n2) / (n1 + n2), 2);
    float R = R0 + (1 - R0) * pow(1-c, 5);

    refractColor *= (1 - R);
    reflectColor *= R;*/
    
    reflectColor *= 1 - object->getFilter();
    refractColor *= object->getFilter();

    Shade color = refractColor + reflectColor;

    color.clamp();
 
    return color;
}

HitRecord Camera::intersectRay(shared_ptr<Object> avoid, const Vector3f& loc, const Vector3f& ray, shared_ptr<Scene> scene)
{
    return scene->intersections(avoid, loc, ray);
}

Vector3f Camera::calcRefractionRay(float n1, float n2, const Vector3f& ray, const Vector3f& normal)
{
    float iorDiv = n1 / n2;
    float c = -ray.dot(normal); // Possible error here
    float radicand = 1 - iorDiv*iorDiv * (1 - c*c);
  
    if (radicand < 0) { return Vector3f(0, 0, 0); } // Internal reflection
    
    return (iorDiv*ray) + (iorDiv*c - sqrt(radicand))*normal;
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

Vector3f Camera::cookTorrance(shared_ptr<Object> object, shared_ptr<Light> light, const Vector3f& ray, const Vector3f& N)
{
    Vector3f V = -ray;
    Vector3f L = light->getPosition();
   
    float NdotV = N.dot(V);
    float NdotL = N.dot(L);

    Vector3f H = L + V;
    H.normalize();

    float NdotHNoClamp = N.dot(H);
    float tmp1 = 2 * (NdotHNoClamp)*(NdotV) / (V.dot(H));
    float tmp2 = 2 * (NdotHNoClamp)* (NdotL) / (V.dot(H));
    float G = std::min(1.0f, std::min(tmp1, tmp2));
    
    float F0 = ((1.0 - object->getIOR()) / (1.0 + object->getIOR()))
        * ((1.0 - object->getIOR()) / (1.0 + object->getIOR()));
    float theta = acos(NdotV);
    float F = F0 + (1 - F0) * std::pow(cos(theta), 5);
    
    float a = acos(NdotHNoClamp);
    float tan2a = tan(a) * tan(a);
    float cos4a = cos(a) * cos(a) * cos(a) * cos(a);
    float m = object->getRoughness();
    if (m <= 0.0f) { m = 0.01; }
    float D = std::exp(-tan2a / (m*m)) / (M_PI * m*m*cos4a);
    
    float kspec = D*F*G / (4 * NdotV * (N.dot(L)));

    Vector3f objCol = object->getRGB();    
    Vector3f color = light->getColor() * kspec * object->getSpecular();

    color(0) *= objCol(0);
    color(1) *= objCol(1);
    color(2) *= objCol(2);
    
    return color;
}

Vector3f Camera::calcMonteCarlo(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& normal, const Vector3f& hitPoint, int gillumiter)
{
    int numRays;
    
    if (gillumiter == IND_LIGHT_BOUNCES) {
        numRays = FIRST_GILL;
    } else if (gillumiter == IND_LIGHT_BOUNCES - 1) {
        numRays = SECOND_GILL;
    } else {
        return calcAmbient(object);
    }
    
    // Calc transform matrix
    Vector3f zUnit = Vector3f(0,0,1);
    float angle = acos(normal.dot(zUnit));
    Vector3f axis = zUnit.cross(normal);
    Matrix3f R;
    R = AngleAxisf(angle, axis);
    
    float u1, u2;

    Vector3f ambient;
    
    for (int i = 0; i < numRays; i++) {
        
        u1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        u2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r = sqrt(u1);
        float theta = M_PI * u2;
        float dx = r*cos(2.0*theta);
        float dy = r*sin(theta);
        float z = sqrt(max(0.0f, 1.0f - dx*dx - dy*dy));
        Vector3f genRay = Vector3f(dx,dy,z);
        
        genRay = R * genRay;

        if (genRay.dot(normal) < 0) {
            cout << "gi ray is more than 90 deg off" << endl; 
        }

        Shade s = castRay(object, hitPoint, genRay, scene, !UNIT_TEST, REFL_RECURSES, NULL, GLOBAL_ILLUM, gillumiter-1);
        Vector3f a = s.getColor();
        
        ambient += genRay.dot(normal)*a;
    }

    ambient *= object->getAmbient();    
    ambient /= numRays;
    
    return ambient;
}

