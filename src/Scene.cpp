#include "Scene.h"

#include "Object.h"
#include "Plane.h"
#include "Light.h"
#include "HitRecord.h"
#include "BVHTree.h"

#include <vector>
#include <memory>
#include <iostream>
#include <limits>
#include <utility>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::addObject(shared_ptr<Object> object)
{
    objects.push_back(object);
}

void Scene::addPlane(shared_ptr<Plane> plane) 
{
    planes.push_back(plane);
}

void Scene::addLight(shared_ptr<Light> light)
{
    lights.push_back(light);
}

vector<shared_ptr<Light> > Scene::getLights()
{
    return lights;
}

void Scene::printObjects()
{
    cout << endl;

    for (unsigned int i = 0; i < lights.size(); i++) {
        lights[i]->print();
        cout << endl;
    }
    
    cout << endl;

    for (unsigned int i = 0; i < objects.size(); i++) {
        objects[i]->print();
        cout << endl;
    }
}

HitRecord Scene::intersections(shared_ptr<Object> avoid, const Vector3f& p0, const Vector3f& d)
{
    HitRecord hrTree;
    bool hitTree;
    
    // Check tree
    hitTree = bvhTree.intersection(avoid, p0, d, &hrTree);

    // Check planes
    float best = numeric_limits<float>::infinity();
    float t;
    shared_ptr<Object> object;

    for (unsigned int i = 0; i < planes.size(); i++) {
        float t = planes[i]->intersection(p0, d);

        if ( t >= 0 && t < best && planes[i] != avoid ) {
            best = t;
            object = planes[i];
        }
    }

    // REFACTOR LATER

    if (!hitTree && best < 0) {
        
        return HitRecord(-1.0, NULL);
    
    } else {
       
        if (!hitTree) { return HitRecord(best, object); }
        if (best < 0) { return hrTree; }
        
        return hrTree.getT() < best ? hrTree : HitRecord(best, object);
    }
}


