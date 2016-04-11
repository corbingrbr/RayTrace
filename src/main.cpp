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

// Creates image using scene and lighting
void rayTrace()
{
    // camera->unitTests(window, scene);
    camera->castRays(window, scene);
}

// Writes image out
void writeImage(string povfile)
{
    int ndx = povfile.find_last_of("/");
    string image = povfile.substr(ndx, povfile.length());
    image = "../images" + image.substr(0, image.find(".")) + ".tga";

    window->genImage(image);
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

    // camera->print();
    // scene->printObjects();

    if (success) {
    
        rayTrace();

        writeImage("../images/" + string(argv[3]));
    }

    return 0;
}
