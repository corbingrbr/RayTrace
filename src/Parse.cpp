#include "Parse.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <Eigen/Dense>

#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"
#include "Scene.h"
#include "Triangle.h"
#include "Pigment.h"
#include "Finish.h"

using namespace std;
using namespace Eigen;

extern shared_ptr<Camera> camera;
extern shared_ptr<Scene> scene;

ifstream Parse::in;
stringstream Parse::ss;
string Parse::line;
string Parse::token;

bool Parse::parse(char *file)
{
    // Open file and set up stream
    in.open(file);
    
    if (!in.good()) {
        cout << "Cannot read " << file << endl;
        return false;
    }
    
    cout << endl;
    cout << "Parsing ... "; 
    
    while (1) {

        nextLine();
      
        if (in.eof()) { break; }
        
        if(line.size() < 2) { continue; }
        
        if (line.at(0) == '/') { continue; }
        
        getToken();
            
        if (token == "camera") { 
            camera = parseCamera(); 
        }
        else if (token == "light_source") { 
            scene->addLight(parseLight()); 
        }
        else if (token == "sphere") { 
            scene->addObject(parseSphere()); 
        }
        else if (token == "plane") { 
            scene->addObject(parsePlane()); 
        }
        else if (token == "triangle") { 
            scene->addObject(parseTriangle()); 
        }
        else {
            cout << "Error: Unknown Statement ->" << token << endl;
            
            return false;
        }
        
    }
    
    cout << "Done" << endl;

    return true;
}

void Parse::getToken()
{
    ss >> token;
}

void Parse::emptyStream()
{
    ss.clear();//clear any bits set
    ss.str(string());
}

void Parse::nextLine()
{
    getline(in, line);
    emptyStream();
    ss << line;
}
    
void Parse::skipRest()
{
    getToken();
    
    while (token != "}") {
        nextLine();
        getToken();
    }
    
    nextLine();
}

 shared_ptr<Camera> Parse::parseCamera()
{
    nextLine();
    ss >> token; // Discard first word
    Vector3f location = parseVector3f();
    
    nextLine();
    ss >> token; // Discard first word
    Vector3f up = parseVector3f();

    nextLine();
    ss >> token; // Discard first word
    Vector3f right = parseVector3f();
    
    nextLine();
    ss >> token; // Discard first word
    Vector3f look_at = parseVector3f();

    nextLine();

    return make_shared<Camera>(location, up, right, look_at);
}

shared_ptr<Light> Parse::parseLight()
{
    getToken(); // Curly brace
    Vector3f position = parseVector3f();
    
    getToken(); // color
    getToken(); // rgb
    
    Vector3f color = parseVector3f();
    
    nextLine();
    
    return make_shared<Light>(position, color);
}

shared_ptr<Sphere> Parse::parseSphere()
{
    getToken(); // CurlyBrace
    Vector3f position = parseVector3f();
    
    getToken();
    float radius = atof(token.c_str());
    
    nextLine();
    
    shared_ptr<Pigment> pigment = parsePigment();  
    nextLine();

    shared_ptr<Finish> finish = parseFinish();

    skipRest();
    
    return make_shared<Sphere>(position, radius, pigment, finish);
}

shared_ptr<Plane> Parse::parsePlane()
{
    getToken(); // {

    Vector3f normal = -parseVector3f();
    getToken();
    float distance = -atof(token.c_str());
    
    nextLine();

    shared_ptr<Pigment> pigment = parsePigment();
    
    nextLine();

    shared_ptr<Finish> finish = parseFinish();
    
    skipRest();
   
    return make_shared<Plane>(normal, distance, pigment, finish);
}

shared_ptr<Triangle> Parse::parseTriangle()
{
     auto vertices = new Vector3f[3];

     nextLine();
     vertices[0] = parseVector3f();
     nextLine();
     vertices[1] = parseVector3f();
     nextLine();
     vertices[2] = parseVector3f();
     nextLine();

     shared_ptr<Pigment> pigment = parsePigment();
     
     nextLine();
     
     shared_ptr<Finish> finish = parseFinish();

     shared_ptr<Triangle> triangle = make_shared<Triangle>(vertices, pigment, finish);
     triangle->init();
     
     return triangle;
}

Vector3f Parse::parseVector3f()
{
    float x, y, z;
    string s;

    ss >> s;
    s = s.substr(1, s.find(","));
    x = atof(s.c_str());

    ss >> s;
    s = s.substr(0, s.find(","));
    y = atof(s.c_str());

    ss >> s;
    s = s.substr(0, s.find(">"));
    z = atof(s.c_str());
    
    return Vector3f(x,y,z);
}

Vector4f Parse::parseVector4f()
{
    float x, y, z, w;
    string s;

    ss >> s;
    s = s.substr(1, s.find(","));
    x = atof(s.c_str());

    ss >> s;
    s = s.substr(0, s.find(","));
    y = atof(s.c_str());

    ss >> s;
    s = s.substr(0, s.find(","));
    z = atof(s.c_str());

    ss >> s;
    s = s.substr(0, s.find(">"));
    w = atof(s.c_str());
    
    return Vector4f(x,y,z,w);
}

shared_ptr<Pigment> Parse::parsePigment()
{
    string s;

    ss >> s; // pigment
    ss >> s; // {
    ss >> s; // color
    ss >> s; // rgb || rgbf

    if (s == "rgbf") {
        Vector4f rgbf = parseVector4f();
        return make_shared<Pigment>(rgbf.head(3), rgbf(3));
    }
    
     Vector3f rgb = parseVector3f();

     return make_shared<Pigment>(rgb, 0.0);
}

shared_ptr<Finish> Parse::parseFinish()
{
    string s;
    
    float ambient = 0.0;
    float diffuse = 0.0;
    float specular = 0.0;
    float roughness = 0.0;
    float reflection = 0.0;
    float refraction = 0.0;
    float ior = 0.0;

    ss >> s; // finish
    ss >> s; // {
    ss >> s;
    

    while (s != "}") {

        string type = s;

        cout << type << endl;

        ss >> s;

        if (type == "ambient") {
            ambient = atof(s.c_str());
        } else if (type == "diffuse") {
            diffuse = atof(s.c_str());
        } else if (type == "specular") {
            specular = atof(s.c_str());
        } else if (type == "roughness") {
            roughness = atof(s.c_str());
        } else if (type == "reflection") {
            reflection = atof(s.c_str());
        } else if (type == "refraction") {
            refraction = atof(s.c_str());
        } else if (type == "ior") {
            ior = atof(s.c_str());
        } else {
            cout << "Encountered unkown pigment component " << type << endl;
        }

        ss >> s;
    }

    return make_shared<Finish>(ambient, diffuse, specular, roughness, reflection, refraction, ior);
}
