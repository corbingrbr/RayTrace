#include "Scene.h"

#include "Object.h"

#include <vector>
#include <memory>
#include <iostream>
#include <limits>

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

void Scene::printObjects()
{
    for (unsigned int i = 0; i < objects.size(); i++) {
        objects[i]->print();
        cout << endl;
    }
}

shared_ptr<Object> Scene::intersections(Vector3f p0, Vector3f d)
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

    return object;
}

