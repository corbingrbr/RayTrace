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

pair<float, shared_ptr<Object> > Scene::intersections(shared_ptr<Object> avoid, const Vector3f& p0, const Vector3f& d)
{
    float best = numeric_limits<float>::infinity();
    float t;
    shared_ptr<Object> hitObject = NULL;

    Vector4f pXForm = Vector4f(p0(0), p0(1), p0(2), 1);
    Vector4f dXForm = Vector4f(d(0), d(1), d(2), 0);

    for (unsigned int i = 0; i < objects.size(); i++) {
        
        if (objects[i] != avoid) {
            Matrix4f inv = objects[i]->getInvXForm();
            
            Vector4f modelp0 = inv * pXForm;
            Vector4f modeld = inv * dXForm;

            t = objects[i]->intersection(modelp0.head(3), modeld.head(3));
             
             if ( t >= 0.0f && t < best) {
                 best = t;
                 hitObject = objects[i];
             }
        }
    }

    return make_pair(best, hitObject);
}

