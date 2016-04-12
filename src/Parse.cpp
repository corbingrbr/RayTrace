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
            
        if (token == "camera") { camera = parseCamera(); }
        else if (token == "light_source") { parseLight(); }
        else if (token == "sphere") { 
            scene->addObject(parseSphere()); }
        else if (token == "plane") { scene->addObject(parsePlane()); }
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
    Vector3f location = parseVector();
    
    nextLine();
    ss >> token; // Discard first word
    Vector3f up = parseVector();

    nextLine();
    ss >> token; // Discard first word
    Vector3f right = parseVector();
    
    nextLine();
    ss >> token; // Discard first word
    Vector3f look_at = parseVector();

    nextLine();

    return make_shared<Camera>(location, up, right, look_at);
}

shared_ptr<Light> Parse::parseLight()
{
    return NULL;
}

shared_ptr<Sphere> Parse::parseSphere()
{
    getToken(); // CurlyBrace
    Vector3f position = parseVector();
    
    getToken();
    float radius = atof(token.c_str());
    
    nextLine();
    
    Vector3f pigment = parsePigment();  
    nextLine();

    skipRest();
    
    return make_shared<Sphere>(position, radius, pigment);
}

shared_ptr<Plane> Parse::parsePlane()
{
    getToken(); // {
    // Negative normal and distance !!!!
    Vector3f normal = -parseVector();
    getToken();
    float distance = -atof(token.c_str());
    
    nextLine();

    Vector3f pigment = parsePigment();
    
    nextLine();

    skipRest();

    return make_shared<Plane>(normal, distance, pigment);
}

Vector3f Parse::parseVector()
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

Vector3f Parse::parsePigment()
{
    string s;
    
    ss >> s; // pigment
    ss >> s; // {color
    ss >> s; // rgb
    
    return parseVector(); // <color>
}
