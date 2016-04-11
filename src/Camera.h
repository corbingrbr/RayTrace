#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Eigen/Dense>
#include <memory>

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
    
    Eigen::Vector3f calcRay(int i, int j, std::shared_ptr<Window> window);
    std::shared_ptr<Object> castRay(Eigen::Vector3f ray, std::shared_ptr<Scene> scene);

    Eigen::Vector3f location;
    Eigen::Vector3f up;
    Eigen::Vector3f right;
    Eigen::Vector3f lookAt;

};

#endif
