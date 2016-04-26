#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Eigen/Dense>
#include <memory>
#include <utility>

class Window;
class Scene;
class Object;

class Camera 
{
    
public:
    
    Camera(Eigen::Vector3f _location, Eigen::Vector3f _up, Eigen::Vector3f _right, Eigen::Vector3f _lookAt);
    virtual ~Camera();
    void castRays(std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    void unitTests(std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    void print();

private:
    
    void unitTest(int i, int j, std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    Eigen::Vector3f calcRay(int i, int j, std::shared_ptr<Window> window);
    std::pair<float, std::shared_ptr<Object> > castRay(Eigen::Vector3f ray, std::shared_ptr<Scene> scene);
    std::pair<float, std::shared_ptr<Object> > castShadowRay(Eigen::Vector3f loc, Eigen::Vector3f ray, std::shared_ptr<Scene> scene);

    Eigen::Vector3f mult2Vecs(Eigen::Vector3f a, Eigen::Vector3f b);
    
    void printVec(std::string name, Eigen::Vector3f v);


    Eigen::Vector3f location;
    Eigen::Vector3f up;
    Eigen::Vector3f right;
    Eigen::Vector3f lookAt;

};

#endif
