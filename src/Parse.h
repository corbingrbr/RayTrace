#ifndef _PARSE_H_
#define _PARSE_H_

#include <memory>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <sstream>


class Camera;
class Light;
class Sphere;
class Plane;
class Triangle;
class Pigment;
class Finish;

class Parse {
    
public:
    
    static bool parse(char *file);
    
private:
    
    static void getToken();
    static void emptyStream();
    static void nextLine();
    static void skipRest();
    static std::shared_ptr<Camera> parseCamera();
    static std::shared_ptr<Light> parseLight();
    static std::shared_ptr<Sphere> parseSphere();
    static std::shared_ptr<Plane> parsePlane();
    static std::shared_ptr<Triangle> parseTriangle();

    static Eigen::Vector3f parseVector3f();
    static Eigen::Vector4f parseVector4f();
    static std::shared_ptr<Pigment> parsePigment();
    static std::shared_ptr<Finish> parseFinish();

    static std::ifstream in;
    static std::stringstream ss;
    static std::string line;
    static std::string token;
};

#endif
