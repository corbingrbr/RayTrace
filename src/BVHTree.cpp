#include "BVHTree.h"

#include "BoundingBox.h"
#include "HitRecord.h"

#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <algorithm>

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
    // Wrap objects up into bounding boxes
    boxUp(objects);
    
    // Generate tree
    constructTree();     
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
            
            float t = node->bb->getObject()->intersection(pos, ray);
            
            if (t >= 0 && node->bb->getObject() != avoid) {
                *hitRecord = HitRecord(t, node->bb->getObject());
                return HIT;
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

void *BVHTree::getSortAlg(int axis) {
    switch (axis) {
    case X_AXIS : return (void *)sortAlgX;
    case Y_AXIS : return (void *)sortAlgY;
    case Z_AXIS : return (void *)sortAlgZ;
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

    if (start - end == 0) { 
        return NULL; 
   
    } else if (start - end == 1) {
        
        node = new BVHNode();
        node->left = NULL;
        node->right = NULL;
        node->bb = &(boxes[start]);
    
    } else {
    
        // Sort boxes by provided axis
        //sort(boxes.begin() + start, boxes.begin() + end, getSortAlg(axis));
        sort(boxes.begin() + start, boxes.begin() + end, sortAlgX);        
        int middle = (start - end) / 2;
        
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
