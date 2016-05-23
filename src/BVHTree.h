#ifndef _BVH_TREE_H_
#define _BVH_TREE_H_

#include "BoundingBox.h"
#include <Eigen/Dense>
#include <memory>
#include <vector>

class HitRecord;

typedef bool (*sortAlg)(BoundingBox, BoundingBox);

struct BVHNode 
{
    BVHNode *left;
    BVHNode *right;
    BoundingBox *bb;
};

class BVHTree {

public:
    BVHTree();
    virtual ~BVHTree();
    void build(std::vector<std::shared_ptr<Object> > objects);
    bool intersection(std::shared_ptr<Object> avoid, const Eigen::Vector3f& pos, const Eigen::Vector3f& ray, HitRecord *hitRecord);
    
    void destroy();

private:
 
    enum Axis { X_AXIS, Y_AXIS, Z_AXIS, NUM_AXIS };
    enum Ndx { X, Y, Z };
 
    sortAlg getSortAlg(int axis);

    static bool sortAlgX(BoundingBox b1, BoundingBox b2);
    static bool sortAlgY(BoundingBox b1, BoundingBox b2);
    static bool sortAlgZ(BoundingBox b1, BoundingBox b2);

    bool intersectHelp(std::shared_ptr<Object> avoid, const Eigen::Vector3f& pos, const Eigen::Vector3f& ray, BVHNode *node, HitRecord *hitRecord);
    void boxUp(std::vector<std::shared_ptr<Object> > objects);
    void constructTree();
    BVHNode *constructHelp(int start, int end, int axis);
    BoundingBox *combine(BoundingBox *left, BoundingBox *right);

    BVHNode *root;
    std::vector<BoundingBox> boxes;

};

#endif
