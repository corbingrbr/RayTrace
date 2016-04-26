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

pair<float, shared_ptr<Object> > Scene::intersections(Vector3f p0, Vector3f d)
{
    float best = numeric_limits<float>::infinity();
    float t;
    shared_ptr<Object> object = NULL;

    for (unsigned int i = 0; i < objects.size(); i++) {
        
        t = objects[i]->intersection(p0, d);

        if ( t >= 0.0f && t < best) {
            best = t;
            object = objects[i];
        }
    }

    return make_pair(best, object);
}

