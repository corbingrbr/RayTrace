#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <memory>
#include <utility>
#include "BVHTree.h"

#include <Eigen/Dense>

class Object;
class Light;
class HitRecord;
class Plane;
class BVHTree;

class Scene {
  
public:
    
    Scene();
    virtual ~Scene();
    void init();
    void addObject(std::shared_ptr<Object> object);
    void addPlane(std::shared_ptr<Plane> plane);
    void addLight(std::shared_ptr<Light> light);
    std::vector<std::shared_ptr<Light> > getLights();
    void printObjects();
    HitRecord intersections(std::shared_ptr<Object> avoid, const Eigen::Vector3f& p0, const Eigen::Vector3f& d);
  
private:

    std::vector<std::shared_ptr<Object> > objects;
    std::vector<std::shared_ptr<Plane> > planes;
    std::vector<std::shared_ptr<Light> > lights;

    BVHTree bvhTree;

};

#endif
