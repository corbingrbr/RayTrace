#include <iostream>
#include <string>
#include <memory>

#include <Eigen/Dense>

#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "Sphere.h"
#include "Parse.h"

using namespace std;
using namespace Eigen;

shared_ptr<Camera> camera;
shared_ptr<Window> window;
shared_ptr<Scene> scene;

// Constructs scene from supplied input file
/*void parse(string file)
{
    // Construct camera and add objects to scene
    camera = make_shared<Camera>(Vector3f(0, 0, 14),
                                 Vector3f(0, 1, 0),
                                 Vector3f(1.33333, 0, 0),
                                 Vector3f(0, 0, 0));


    scene = make_shared<Scene>();
    
    scene->addObject(make_shared<Sphere>(Vector3f(0, 0, 0), 1.01, Vector3f(1.0, 1.0, 1.0)));
    scene->addObject(make_shared<Sphere>(Vector3f(0, 0, -1.5), 2.25, Vector3f(0, 0, 0)));
    scene->addObject(make_shared<Sphere>(Vector3f(0, 0, -4.5), 3.75, Vector3f(.25, .25, .25)));
    scene->addObject(make_shared<Sphere>(Vector3f(-5.5, 3, 0), 1.1, Vector3f(.12, .34, .56)));
    }*/

// Creates image using scene and lighting
void rayTrace()
{
    // camera->unitTests(window, scene);
    camera->castRays(window, scene);
}

// Writes image out
void writeImage(string povfile)
{
    int lastindex = povfile.find_last_of("."); 
    string outfile = povfile.substr(0, lastindex) + ".tga"; 

    window->genImage(outfile);
}

int main(int argc, char **argv)
{
    if (argc < 4 || argc >= 5) { 
        cout << "Must supply width, height, and input_file" << endl;
        return 0;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    char *file = argv[3];

    window = make_shared<Window>(width, height);
    scene = make_shared<Scene>();

    bool success = Parse::parse(file);

    camera->print();
    cout << endl;
    scene->printObjects();

    if (success) {
    
        //rayTrace();

        //writeImage("../images/" + string(argv[3]));
    }

    return 0;
}
