#include "Parse.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
#include <Eigen/Dense>
#include <algorithm>

#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"
#include "Scene.h"
#include "Triangle.h"
#include "Pigment.h"
#include "Finish.h"
#include "Tools.h"

using namespace std;
using namespace Eigen;

extern shared_ptr<Camera> camera;
extern shared_ptr<Scene> scene;

ifstream Parse::in;
stringstream Parse::ss;
string Parse::line;
string Parse::token;

bool Parse::parse(char *file, int aa_res, bool jitter)
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
            camera = parseCamera(aa_res, jitter);
        }
        else if (token == "light_source") { 
            scene->addLight(parseLight()); 
        }
        else if (token == "sphere") { 
            scene->addObject(parseSphere()); 
        }
        else if (token == "plane") { 
            scene->addPlane(parsePlane()); 
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
    emptyStream();
    getline(in, line);
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

shared_ptr<Camera> Parse::parseCamera(int aa_res, bool jitter)
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

    return make_shared<Camera>(location, up, right, look_at, aa_res, jitter);
}

shared_ptr<Light> Parse::parseLight()
{
    Vector3f position = parseVector3f();
    
    getToken(); // color
    getToken(); // rgb
    
    Vector3f color = parseVector3f();
    
    return make_shared<Light>(position, color);
}

///////////////////////////////////////////////////////////////////////

shared_ptr<Sphere> Parse::parseSphere()
{
    Vector3f position = parseVector3f();
    
    getToken();
    float radius = atof(token.c_str());
    nextLine();
    
    shared_ptr<Pigment> pigment;
    shared_ptr<Finish> finish;
    Matrix4f xform = Matrix4f::Identity();

    getToken();

    while (token != "}") {
        
        if (token == "scale") {
            xform = scale(parseVector3f()) * xform;
        } else if (token == "rotate") {
            // Only rotates about one axis at a time
            xform = rotate(parseVector3f()) * xform;
        } else if (token == "translate"){
            xform = translate(parseVector3f()) * xform;
        } else if (token == "pigment") {
            pigment = parsePigment();
        } else if (token == "finish") {
            finish = parseFinish();
        } else {
            cout << "PARSE ERROR: Unknown Attribute -> " << token << endl;
        }
        
        nextLine();
        getToken();
    }

    return make_shared<Sphere>(position, radius, pigment, finish, xform.inverse());
}

shared_ptr<Plane> Parse::parsePlane()
{
    Vector3f normal = parseVector3f();
    getToken();
    float distance = atof(token.c_str());
    nextLine();

    shared_ptr<Pigment> pigment;
    shared_ptr<Finish> finish;
    Matrix4f xform = Matrix4f::Identity();

    getToken();

    while (token != "}") {
        
        if (token == "scale") {
            xform = scale(parseVector3f()) * xform;
        } else if (token == "rotate") {
            // Only rotates about one axis at a time
            xform = rotate(parseVector3f()) * xform;
        } else if (token == "translate"){
            xform = translate(parseVector3f()) * xform;
        } else if (token == "pigment") {
            pigment = parsePigment();
        } else if (token == "finish") {
            finish = parseFinish();
        } else {
            cout << "PARSE ERROR: Unknown Attribute -> " << token << endl;
        }
        
        nextLine();
        getToken();
    }

    return make_shared<Plane>(normal, distance, pigment, finish, xform.inverse());
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
    
    shared_ptr<Pigment> pigment;
    shared_ptr<Finish> finish;
    Matrix4f xform = Matrix4f::Identity();
    
    getToken();
    
    while (token != "}") {
        
        if (token == "scale") {
            xform = scale(parseVector3f()) * xform;
        } else if (token == "rotate") {
             // Only rotates about one axis at a time
            xform = rotate(parseVector3f()) * xform;
        } else if (token == "translate"){
            xform = translate(parseVector3f()) * xform;
        } else if (token == "pigment") {
             pigment = parsePigment();
        } else if (token == "finish") {
            finish = parseFinish();
        } else {
            cout << "PARSE ERROR: Unknown Attribute -> " << token << endl;
        }
        
        nextLine();
        getToken();
    }

    shared_ptr<Triangle> triangle = make_shared<Triangle>(vertices, pigment, finish, xform.inverse());
     triangle->init();
     
     return triangle;
}

Matrix4f Parse::translate(Vector3f t)
{
    Matrix4f E = Matrix4f::Identity();
    E(0,3) = t(0);
	E(1,3) = t(1);
	E(2,3) = t(2);

    return E;
}

Matrix4f Parse::rotate(Vector3f r) 
{
    
    float deg2rad = M_PI / 180;
    Matrix4f E = Matrix4f::Identity();


    if (r(0) > 0) { // X - Axis

        E(1,1) = cos(r(0)*deg2rad);
        E(2,1) = sin(r(0)*deg2rad);
        E(1,2) = -sin(r(0)*deg2rad);
        E(2,2) = cos(r(0)*deg2rad);
        
    } else if (r(1) > 0) { // Y - Axis
        
        E(0,0) = cos(r(1)*deg2rad);
        E(2,0) = -sin(r(1)*deg2rad);
        E(0,2) = sin(r(1)*deg2rad);
        E(2,2) = cos(r(1)*deg2rad);

        
    } else { // Z - Axis
        
        E(0,0) = cos(r(2)*deg2rad);
        E(1,0) = sin(r(2)*deg2rad);
        E(0,1) = -sin(r(2)*deg2rad);
        E(1,1) = cos(r(2)*deg2rad);

    }

    return E;
}

Matrix4f Parse::scale(Vector3f s) 
{
    //Tools::printVec3("Scale", s);
  	Matrix4f E = Matrix4f::Identity();
	E(0,0) = s(0);
	E(1,1) = s(1);
	E(2,2) = s(2);
    
	return E;  
}

Vector3f Parse::parseVector3f()
{
    float x, y, z;
    string s;

    ss >> s;
    int ndx = s.at(0) == '{' ? 2 : 1;
    
    s = s.substr(ndx, s.length());
    x = atof(s.c_str());

    ss >> s;
    y = atof(s.c_str());

    ss >> s;
    z = atof(s.c_str());
    
    return Vector3f(x,y,z);
}

Vector4f Parse::parseVector4f()
{
    float x, y, z, w;
    string s;

    ss >> s;
    int ndx = s.at(0) == '{' ? 2 : 1;

    s = s.substr(ndx, s.length());
    x = atof(s.c_str());

    ss >> s;
    y = atof(s.c_str());

    ss >> s;
    z = atof(s.c_str());

    ss >> s;
    w = atof(s.c_str());
    
    return Vector4f(x,y,z,w);
}

shared_ptr<Pigment> Parse::parsePigment()
{
    string s;

    ss >> s; // {color
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

    ss >> s;
    s = s.substr(1, s.length()); // Removal of "{"
    
    while (1) {

        string type = s;
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

        if (s.find("}") != string::npos) { break; } // End of list

        ss >> s;
    }

    return make_shared<Finish>(ambient, diffuse, specular, roughness, reflection, refraction, ior);
}
