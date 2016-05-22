#ifndef _BVH_TREE_H_
#define _BVH_TREE_H_

#include "BoundingBox.h"
#include <Eigen/Dense>
#include <memory>
#include <vector>

class HitRecord;

typedef bool (*fptr)(); // For sort algorithm 

class BVHTree {

public:
    BVHTree();
    virtual ~BVHTree();
    void build(std::vector<std::shared_ptr<Object> > objects);
    bool intersection(Eigen::Vector3f& pos, Eigen::Vector3f& ray, HitRecord *hitRecord);
    void destroy();

private:
 
    enum Axis { X_AXIS, Y_AXIS, Z_AXIS, NUM_AXIS };
    enum Ndx { X, Y, Z };

    struct BVHNode {
        BVHNode *left;
        BVHNode *right;
        BoundingBox *bb;
        
        BoundingBox * getBB() { return bb; }
    };
    
    fprt getSortAlg(int axis);

    static bool sortAlgX(BoundingBox& b1, BoundingBox& b2);
    static bool sortAlgY(BoundingBox& b1, BoundingBox& b2);
    static bool sortAlgZ(BoundingBox& b1, BoundingBox& b2);

    std::vector<BoundingBox> filterPlanes(std::vector<std::shared_ptr<Object> > objects);
    void constructTree(std::vector<BoundingBox>& boxes);
    void constructHelper(std::Vector<BoundingBox>& boxes, int axis);
    

    BVHNode *root;
    std::vector<Objects> planes;

};

#endif
