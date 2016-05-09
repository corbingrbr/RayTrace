#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <memory>
#include <utility>

#include <Eigen/Dense>

class Object;
class Light;

class Scene {
  
public:
    
    Scene();
    virtual ~Scene();
    void addObject(std::shared_ptr<Object> object);
    void addLight(std::shared_ptr<Light> light);
    std::shared_ptr<Light> getLight();
    void printObjects();
    std::pair<float, std::shared_ptr<Object> > intersections(std::shared_ptr<Object> avoid, Eigen::Vector3f p0, Eigen::Vector3f d);
  
private:

    std::vector<std::shared_ptr<Object> > objects;
    std::vector<std::shared_ptr<Light> > lights;

};

#endif
