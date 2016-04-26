#ifndef _PARSE_H_
#define _PARSE_H_

#include <memory>
#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <sstream>

#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"

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

    static Eigen::Vector3f parseVector();
    static Eigen::Vector3f parsePigment();
    static Eigen::Vector4f parseFinish();

    static std::ifstream in;
    static std::stringstream ss;
    static std::string line;
    static std::string token;
};

#endif
