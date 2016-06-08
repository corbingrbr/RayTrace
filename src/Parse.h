#ifndef _PARSE_H_
#define _PARSE_H_

#include <memory>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <sstream>
#include <Eigen/Dense>


class Camera;
class Light;
class Sphere;
class Plane;
class Triangle;
class Box;
class Pigment;
class Finish;

class Parse {
    
public:
    
    static bool parse(char *file, int aa_res, bool jitter, bool gi, bool cartoon, bool isBlinnPhong);
    
private:
    
    static void getToken();
    static void emptyStream();
    static void nextLine();
    static void skipRest();
    static std::shared_ptr<Camera> parseCamera(int aa_res, bool jitter, bool gi, bool cartoon, bool isBlinnPhong);
    static std::shared_ptr<Light> parseLight();
    static std::shared_ptr<Sphere> parseSphere();
    static std::shared_ptr<Plane> parsePlane();
    static std::shared_ptr<Triangle> parseTriangle();
    static std::shared_ptr<Box> parseBox();

    static Eigen::Matrix4f translate(Eigen::Vector3f t);
    static Eigen::Matrix4f rotate(Eigen::Vector3f r);
    static Eigen::Matrix4f scale(Eigen::Vector3f s);

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
