#include <iostream>
#include <string>
#include <memory>

#include <Eigen/Dense>
#include <cstdlib>
#include <time.h>


#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "Sphere.h"
#include "Parse.h"
#include "Shade.h"
#include "Tools.h"

#include "Box.h"
#include "Finish.h"
#include "Pigment.h"

using namespace std;
using namespace Eigen;

#define PRINT_SCENE false
#define TESTING false

// Anti-Aliasing
#define AA_RES 3
#define JITTER true

shared_ptr<Camera> camera;
shared_ptr<Window> window;
shared_ptr<Scene> scene;

// Creates image using scene and lighting

void rayTrace()
{
    camera->castRays(window, scene);
}

void unitTests()
{
    camera->unitTests(window, scene);
}

// Writes image out
void writeImage(string povfile)
{
    int ndx = povfile.find_last_of("/");
    string image = povfile.substr(ndx, povfile.length());
    image = "images" + image.substr(0, image.find(".")) + ".tga";

    window->genImage(image);
}

int main(int argc, char **argv)
{
 
    if (argc < 6 || argc >= 7) { 
        cout << "Must supply width, height, input_file, BRDF, AA" << endl;
        return 0;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    char *file = argv[3];
    bool isBlinnPhong = atoi(argv[4]) == 0 ? true : false;
    int aa = atoi(argv[5]) == 1 ? true : false;

    bool gi = false;
    bool cartoon = false;
   
    
    window = make_shared<Window>(width, height);
    scene = make_shared<Scene>();
    
    bool success = aa ? Parse::parse(file, 3, JITTER, gi, cartoon, isBlinnPhong) : 
        Parse::parse(file, 1, !JITTER, gi, cartoon, isBlinnPhong);

    if (JITTER) { srand(time(NULL)); } // Seed random number generator

    if (success) {
        
        if (PRINT_SCENE) { scene->printObjects(); }

        scene->init(); // Construct BVHTree

        if (TESTING) {

            unitTests();

        } else {

            rayTrace();
            writeImage("../images/" + string(argv[3]));
        }
        
    }
    
    cout << endl;

    return 0;
}
