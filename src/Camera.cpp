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
                shade = castRay(NULL, location, rays[k], scene, !UNIT_TEST, REFL_RECURSES, NULL, PRIMARY);
                color += shade.getColor();
            }
            
            color /= rays.size();

            // Set color of pixel
            window->setPixel(i, j, color);
        }
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

    //unitTest(399, 280, window, scene);
    //unitTest(240, 280, window, scene);
    unitTest(417, 303, window, scene);
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
    
    HitRecord hitRecord = intersectRay(avoid, hitPoint, feeler, scene);

    return hitRecord.getObject() != NULL && (hitRecord.getT() * feeler).norm() < hit2Light.norm();
}

Shade Camera::calcLocal(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& hitPoint)
{
    Vector3f ambient = Vector3f(0,0,0);
    Vector3f diffuse = Vector3f(0,0,0);
    Vector3f specular = Vector3f(0,0,0);

    // Iterate through lights in scene
    vector<shared_ptr<Light> > lights = scene->getLights();

    for (int i = 0; i < lights.size(); i++) {
        
        if (!isShadowed(scene, lights[i], object, hitPoint)) {
            
            // Light vector
            Vector3f hit2Light = lights[i]->getPosition() - hitPoint;
            Vector3f feeler = hit2Light.normalized();
            Vector3f normal = object->getNormal(hitPoint);
            
            diffuse += calcDiffuse(object, normal, feeler, lights[i]);
            specular += calcSpecular(object, hitPoint, normal, feeler, lights[i]);
        } 
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
        
        refractRay = calcRefractionRay(n1, n2, ray, -normal); 
        
        // Internal Reflection
        if (refractRay(0) == 0 && refractRay(1) == 0 && refractRay(2) == 0) { 
            return reflectColor;
        }    

        c = ray.dot(normal);
    }
    
    Vector3f newLoc = hitPoint + refractRay * EPSILON;

    refractColor = castRay(NULL, newLoc, refractRay, scene, unitTest, iteration-1, log, REFRACT);
   
    float R0 = pow((n1 - n2) / (n1 + n2), 2);
    float R = R0 + (1 - R0) * pow(1-c, 5);

    refractColor *= (1 - R);
    reflectColor *= R;
    
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

Shade Camera::calcMonteCarlo(shared_ptr<Scene> scene, shared_ptr<Object> object, const Vector3f& normal, const Vector3f& hitPoint, int gillumiter)
{
    int numRays;
    
    if (gillumiter == IND_LIGHT_BOUNCES) {
        numRays = 128;
    } else if (gillumiter == IND_LIGHT_BOUNCES - 1) {
        numRays = 32;
    } else {
        return Shade(calcAmbient(object), Vector3f(0,0,0), Vector3f(0,0,0));
    }
    
    // Calc transform matrix
    float angle = acos(normal.dot(Vector3f(0, 0, 1)));
    Vector3f axis = normal.cross(Vector3f(0,0,1));
    Matrix3f R;
    R = AngleAxisf(angle, axis);
    
    float u1, u2;

    Shade finalShade = Shade();
    
    for (int i = 0; i < numRays; i++) {
        
        u1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        u2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float r = sqrt(1.0f - u1 * u1);
        float phi = 2 * M_PI * u2;

        Vector3f genRay = Vector3f(cos(phi) * r, sin(phi) * r, u1);
        
        genRay = R * genRay;
        Shade s = castRay(object, hitPoint, genRay, scene, !UNIT_TEST, REFL_RECURSES, NULL, GLOBAL_ILLUM);
        
        finalShade = finalShade + s;
    }
    
    finalShade /= numRays;
    
    return finalShade;
}

// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
/*Eigen::Vector3f CookTorrance(const Shape *hitShape, const Eigen::Vector3f &n, const Eigen::Vector3f &l, 
                                        const Eigen::Vector3f &d, const Eigen::Vector3f &lightCol, 
                                        double curIOR, double newIOR) {
    Eigen::Vector3f v = -d;
    Eigen::Vector3f h = (l + v).normalized();
    float m = hitShape->finish.roughness;
    v.normalize();

    float NdotV= n.dot(v);
    float NdotH= n.dot(h);
    float NdotL= n.dot(l);
    float VdotH= v.dot(h);

    // Calculate Cook-Torrance (G)
    double G1 = (2.0*NdotH*NdotV)/VdotH;
    double G2 = (2.0*NdotH*NdotL)/VdotH;
    float Geom = fmin(1.0, fmin(G2, G1));

    // Calculate GGX (Trowbridge-Reitz) Distribution (D)
    double alpha = m*m;
    double alphaSqr = alpha*alpha;
    double denom = PI*pow(((NdotH*NdotH)*(alphaSqr - 1) + 1), 2);
    float Rough= alphaSqr/denom;

    // Calculate F
    double r1 = curIOR; // index of refraction for the medium one (like Air,  which is 1)
    double r2 = newIOR; // index of refraction for the medium two (like lead, which is 2)
    float R0 = pow(((r1 - r2)/(r1 + r2)), 2);
    float Fresnel= R0 + (1.0 - R0)*pow(1.0 - VdotH, 5.0);   //schlick's approximation

    double ct = (Rough*Fresnel*Geom)/(4.0*NdotV*NdotL);
    double spec = (hitShape->finish.specular)*ct;

    // calculate diffuse
    double dif = hitShape->finish.diffuse*max(0.0f, NdotL);

    Eigen::Vector3f ret = hitShape->color.head<3>()*(dif + spec);
    ret[0] *= lightCol[0];
    ret[1] *= lightCol[1];
    ret[2] *= lightCol[2];

    return ret;
}

/*Eigen::Vector3f ToonSorta(const Shape *hitShape, const Eigen::Vector3f &n, const Eigen::Vector3f &l, 
                                const Eigen::Vector3f &d, const Eigen::Vector3f &lightCol, 
                                Eigen::Vector3f *retColor) {
    Eigen::Vector3f baseColor = hitShape->color.head<3>();

    if (-.1 < d.dot(n)) {
        // we are at an edge and should color it black for outlining
        *retColor = Eigen::Vector3f(0,0,0);
        return Eigen::Vector3f(0,0,0);
    }

    // prepare what is need for blinn-phong
    Eigen::Vector3f h = l + -d;
    h.normalize();

    // make diffuse
    double ndl = max(0.0f, n.dot(l));
    Eigen::Vector3f dif = baseColor*hitShape->finish.diffuse*ndl;
    
    // make specular
    double hdn = h.dot(n);
    double shine = (1.0f/hitShape->finish.roughness);
    Eigen::Vector3f spec = (baseColor*hitShape->finish.specular)*pow(hdn, shine);

    // add in light color
    Eigen::Vector3f ret = dif + spec;

    ret[0] *= lightCol[0];
    ret[1] *= lightCol[1];
    ret[2] *= lightCol[2];

    return ret;
    }*/
