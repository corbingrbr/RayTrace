#include "BVHTree.h"

#include "BoundingBox.h"
#include "HitRecord.h"

#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

#define HIT true

using namespace std;
using namespace Eigen;

BVHTree::BVHTree()
    : root(NULL)
{
}

BVHTree::~BVHTree()
{
}

void BVHTree::build(vector<shared_ptr<Object> > objects)
{
    boxUp(objects); // Wrap objects up into bounding boxes

    constructTree(); // Generate tree  
}

bool BVHTree::intersection(shared_ptr<Object> avoid, const Vector3f& pos, const Vector3f& ray, HitRecord *hr)
{
    return intersectHelp(avoid, pos, ray, root, hr); 
}

bool BVHTree::intersectHelp(shared_ptr<Object> avoid, const Vector3f& pos, const Vector3f& ray, BVHNode *node, HitRecord *hitRecord)
{
    if (node == NULL) { return false; }

    if ((node->bb)->intersection(pos, ray)) {
        // Check if leaf
        if (node->left == NULL && node->right == NULL) {

            shared_ptr<Object> object = node->bb->getObject(); 
            
            if (object != avoid) { // Make sure its not object to avoid
                
                Vector4f pXForm = Vector4f(pos(0), pos(1), pos(2), 1);
                Vector4f dXForm = Vector4f(ray(0), ray(1), ray(2), 0);
                
                Matrix4f inv = object->getInvXForm();
                
                Vector4f modelpos = inv * pXForm;
                Vector4f modelray = inv * dXForm;
                
                float t = object->intersection(modelpos.head(3), modelray.head(3));
                
                if (t >= 0) {
                    *hitRecord = HitRecord(t, object);
                    return HIT;
                } 
            }
              
            return !HIT;

        } else {
            // Either a node is a leaf, or it has two children
             HitRecord lHR, rHR;
             bool lHit, rHit;
             
             lHit = intersectHelp(avoid, pos, ray, node->left, &lHR);
             rHit = intersectHelp(avoid, pos, ray, node->right, &rHR);
             
             if (!lHit && !rHit) { 
                 return !HIT; 
             } else if (!lHit) { 
                 *hitRecord = rHR; 
             } else if (!rHit) { 
                 *hitRecord = lHR; 
             } else {
                 *hitRecord = lHR.getT() < rHR.getT() ? lHR : rHR;
             }
             
             return HIT;    
        }
    }

    return !HIT;
}

void BVHTree::destroy()
{   
}

sortAlg BVHTree::getSortAlg(int axis) {
    switch (axis) {
    case X_AXIS : return sortAlgX;
    case Y_AXIS : return sortAlgY;
    case Z_AXIS : return sortAlgZ;
    }
}

bool BVHTree::sortAlgX(BoundingBox b1, BoundingBox b2)
{
    return b1.getCenter()(X) > b2.getCenter()(X);
}

bool BVHTree::sortAlgY(BoundingBox b1, BoundingBox b2)
{
    return b1.getCenter()(Y) > b2.getCenter()(Y);
}

bool BVHTree::sortAlgZ(BoundingBox b1, BoundingBox b2)
{
    return b1.getCenter()(Z) > b2.getCenter()(Z);
}

void BVHTree::boxUp(vector<shared_ptr<Object> > objects)
{
    for (unsigned int i = 0; i < objects.size(); i++) {
            boxes.push_back(BoundingBox(objects[i]));
            boxes[i].setup();
    }
}

void BVHTree::constructTree() 
{
    root = constructHelp(0, boxes.size(), X_AXIS);
}

BVHNode *BVHTree::constructHelp(int start, int end, int axis)
{
    BVHNode *node;

    if (end - start == 0) { 
        
        return NULL; 
   
    } else if (end - start == 1) {
        
        node = new BVHNode();
        node->left = NULL;
        node->right = NULL;
        node->bb = &(boxes[start]);

    } else {
        // Sort boxes by provided axis
        sort(boxes.begin() + start, boxes.begin() + end, getSortAlg(axis));

        int middle = (end + start) / 2;
        
        node = new BVHNode();
        
        node->left = constructHelp(start, middle, (axis + 1) / NUM_AXIS);
        node->right = constructHelp(middle, end, (axis + 1) / NUM_AXIS);
        node->bb = combine(node->left->bb, node->right->bb);
    }

    return node;
}

BoundingBox *BVHTree::combine(BoundingBox *left, BoundingBox *right)
{
    Vector3f lmin = left->getMin();
    Vector3f lmax = left->getMax();
    Vector3f rmin = right->getMin();
    Vector3f rmax = right->getMax();

    float minx, miny, minz, maxx, maxy, maxz;

    minx = min(lmin(0), rmin(0));
    miny = min(lmin(1), rmin(1));
    minz = min(lmin(2), rmin(2));
    maxx = max(lmax(0), rmax(0));
    maxy = max(lmax(1), rmax(1));
    maxz = max(lmax(2), rmax(2));
      
    return new BoundingBox(Vector3f(minx, miny, minz), Vector3f(maxx, maxy, maxz));
}
