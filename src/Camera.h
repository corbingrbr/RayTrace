#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Eigen/Dense>
#include <memory>
#include <utility>
#include <stack>

class Window;
class Scene;
class Object;
class Light;
class Shade;
class PrintOut;

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
    std::pair<float, std::shared_ptr<Object> > intersectRay(std::shared_ptr<Object> avoid, Eigen::Vector3f loc, Eigen::Vector3f ray, std::shared_ptr<Scene> scene);
    Shade castRay(std::shared_ptr<Object> avoid, Eigen::Vector3f loc, Eigen::Vector3f ray, std::shared_ptr<Scene> scene, bool unitTest, int iteration, std::shared_ptr<std::stack<PrintOut> > log);
    bool isShadowed(std::shared_ptr<Scene> scene, std::shared_ptr<Light> light, std::shared_ptr<Object> avoid, Eigen::Vector3f hitPoint);
    Shade calcLocal(std::shared_ptr<Scene> scene, std::shared_ptr<Object> object, Eigen::Vector3f hitPoint);
    Shade calcReflection(std::shared_ptr<Scene> scene, std::shared_ptr<Object> object, int iteration, Eigen::Vector3f hitPoint, Eigen::Vector3f, bool unitTest, std::shared_ptr<std::stack<PrintOut> > log);
    Shade calcRefraction(std::shared_ptr<Scene> scene
    
    Eigen::Vector3f calcAmbient(std::shared_ptr<Object> object);
    Eigen::Vector3f calcDiffuse(std::shared_ptr<Object> object, Eigen::Vector3f normal, Eigen::Vector3f feeler, std::shared_ptr<Light> light);
    Eigen::Vector3f calcSpecular(std::shared_ptr<Object> object, Eigen::Vector3f hitPoint, Eigen::Vector3f normal, Eigen::Vector3f feeler, std::shared_ptr<Light> light);

    Eigen::Vector3f location;
    Eigen::Vector3f up;
    Eigen::Vector3f right;
    Eigen::Vector3f lookAt;

};

#endif

// 331, 30
