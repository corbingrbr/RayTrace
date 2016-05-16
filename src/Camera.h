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
    
    Camera(Eigen::Vector3f _location, Eigen::Vector3f _up, Eigen::Vector3f _right, Eigen::Vector3f _lookAt, int AA_res, bool jitter);
    virtual ~Camera();
    void castRays(std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    void unitTests(std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    void print();

private:
    
    enum Type { PRIMARY, REFLECT, REFRACT };

    void unitTest(int i, int j, std::shared_ptr<Window> window, std::shared_ptr<Scene> scene);
    Eigen::Vector3f calcRay(float dx, float dy, std::shared_ptr<Window> window);
    std::vector<Eigen::Vector3f> calcRays(int i, int j, std::shared_ptr<Window> window);
    std::pair<float, std::shared_ptr<Object> > intersectRay(std::shared_ptr<Object> avoid, const Eigen::Vector3f& loc, const Eigen::Vector3f& ray, std::shared_ptr<Scene> scene);
    Shade castRay(std::shared_ptr<Object> avoid, const Eigen::Vector3f& loc, const Eigen::Vector3f& ray, std::shared_ptr<Scene> scene, bool unitTest, int iteration, std::shared_ptr<std::stack<PrintOut> > log, int type);
    bool isShadowed(std::shared_ptr<Scene> scene, std::shared_ptr<Light> light, std::shared_ptr<Object> avoid, const Eigen::Vector3f& hitPoint);
    Shade calcLocal(std::shared_ptr<Scene> scene, std::shared_ptr<Object> object, const Eigen::Vector3f& hitPoint);
    bool calcReflection(std::shared_ptr<Scene> scene, std::shared_ptr<Object> object, int iteration, const Eigen::Vector3f& hitPoint, const Eigen::Vector3f& ray, bool unitTest, std::shared_ptr<std::stack<PrintOut> > log, Shade *color);
    Shade calcRefraction(std::shared_ptr<Scene> scene, std::shared_ptr<Object> object, int iteration, const Eigen::Vector3f& hitPoint, const Eigen::Vector3f& ray, bool unitTest, std::shared_ptr<std::stack<PrintOut> > log);  
    Eigen::Vector3f calcRefractionRay(float n1, float n2, const Eigen::Vector3f& ray, const Eigen::Vector3f& normal);
    
    Eigen::Vector3f calcAmbient(std::shared_ptr<Object> object);
    Eigen::Vector3f calcDiffuse(std::shared_ptr<Object> object, const Eigen::Vector3f& normal, const Eigen::Vector3f& feeler, std::shared_ptr<Light> light);
    Eigen::Vector3f calcSpecular(std::shared_ptr<Object> object, const Eigen::Vector3f& hitPoint, const Eigen::Vector3f& normal, const Eigen::Vector3f& feeler, std::shared_ptr<Light> light);

    Eigen::Vector3f location;
    Eigen::Vector3f up;
    Eigen::Vector3f right;
    Eigen::Vector3f lookAt;
   
    float rght, left, top, bottom;
    int AA_res;
    bool jitter;    
};

#endif
