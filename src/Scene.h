#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include <memory>

#include <Eigen/Dense>

class Object;

class Scene {
  
public:
    
    Scene();
    virtual ~Scene();
    void addObject(std::shared_ptr<Object> object);
    void printObjects();
    std::shared_ptr<Object> intersections(Eigen::Vector3f p0, Eigen::Vector3f d);
  
private:

    std::vector<std::shared_ptr<Object> > objects;

};

#endif
