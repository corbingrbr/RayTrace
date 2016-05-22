#include "BVHTree.h"

#include "BoundingBox.h"

#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <algorithm>

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
    // Filter out planes and wrap remaining objects in bounding boxes
    vector<BoundingBox> boxes = filterPlanes(objects);
    
    constructTree(boxes);     
}

bool BVHTree::intersection(Vector3f& pos, Vector3f& ray, HitRecord *hitRecord)
{
    if (root == NULL && planes.size() == 0) { return false; }

    

}

void BVHTree::destroy()
{   
}

fptr BVHTree::getSortAlg(int axis) {
    switch (axis) {
    case X_AXIS : return sortAlgX;
    case Y_AXIS : return sortAlgY;
    case Z_AXIS : return sortAlgZ;
    }
}

bool BVHTree::sortAlgX(BoundingBox& b1, BoundingBox& b2)
{
    return b1.getCenter()(X) > b2.getCenter()(X);
}

bool BVHTree::sortAlgY(BoundingBox& b1, BoundingBox& b2)
{
    return b1.getCenter()(Y) > b2.getCenter()(Y);
}

bool BVHTree::sortAlgZ(BoundingBox& b1, BoundingBox& b2)
{
    return b1.getCenter()(Z) > b2.getCenter()(Z);
}

vector<BoundingBox> BVHTree::filterPlanes(vector<shared_ptr<Object> > objects)
{
    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects[i]->getID() == Object::PLANE) {
            planes.push_back(object[i]);
        } else {
            boxes.push_back(BoundingBox(object[i]));
            boxes[i].setup();
        }
    }
}

void constructTree(vector<BoundingBox>& boxes) 
{
    root = constructTreeHelper(boxes, 0, boxes.size(), X_AXIS);
}

BVHNode *constructTreeHelper(vector<BoundingBox>& boxes, int start, int end, int axis)
{
    BVHNode *node;

    if (start - end == 0) { 
        return NULL; 
   
    } else if (start - end == 1) {
        
        node = new BVHNode();
        node->left = NULL;
        node->right = NULL;
        node->bb = boxes[start];
    
    } else {
    
        // Sort boxes by provided axis
        sort(boxes.begin() + start, boxes.begin() + end, getSortAlg(axis));
        
        int middle = (start - end) / 2;
        
        node = new BVHNode();
        
        node->left = constructTreeHelper(boxes, start, middle, (axis + 1) / NUM_AXIS);
        node->right = constructTreeHelper(boxes, middle, end, (axis + 1) / NUM_AXIS);
        node->bb = combine(node->left.getBB(), node->right.getBB());
    }

    return node;
}

BoundingBox *combine(BoundingBox *left, BoundingBox *right)
{
    Vector3f lmin = left->getMin();
    Vector3f lmax = left->getMax();
    Vector3f rmin = right->getMin();
    Vector3f rmax = right->getMax();

    float minx, miny, minz, maxx, maxy, maxz;

    minx = min(lmin(X), rmin(X));
    miny = min(lmin(Y), rmin(Y));
    minz = min(lmin(Z), rmin(Z));
    maxx = max(lmax(X), rmax(X));
    maxy = max(lmax(Y), rmax(Y));
    maxz = max(lmax(Z), rmax(Z));
      
    return new BoundingBox(Vector3f(minx, miny, minz), Vector3f(maxx, maxy, maxz));
}
