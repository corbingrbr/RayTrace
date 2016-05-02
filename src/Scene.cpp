#include "Scene.h"

#include "Object.h"
#include "Light.h"

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

void Scene::addLight(shared_ptr<Light> light)
{
    lights.push_back(light);
}

shared_ptr<Light> Scene::getLight()
{
    return lights[0];
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

pair<float, shared_ptr<Object> > Scene::intersections(shared_ptr<Object> object, Vector3f p0, Vector3f d)
{
    float best = numeric_limits<float>::infinity();
    float t;
    shared_ptr<Object> hitObject = NULL;

    for (unsigned int i = 0; i < objects.size(); i++) {
        
        if (objects[i] != object) {
            t = objects[i]->intersection(p0, d);
        }

        if ( t >= 0.0f && t < best) {
            best = t;
            hitObject = objects[i];
        }
    }

    return make_pair(best, hitObject);
}

