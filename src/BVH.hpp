//
//  BVH.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/20/17.
//
//

#ifndef BVH_hpp
#define BVH_hpp

#include <vector>
#include <memory>

#include <glm/gtc/type_ptr.hpp>

class GameObject;
class AABoundingBox;

class BVH
{
public:
    BVH(const std::vector<std::shared_ptr<GameObject>> &objects);
    virtual ~BVH();
    
    void buildTree(const std::vector<std::shared_ptr<GameObject>> &objects);

    std::shared_ptr<GameObject> findClosestHitObject(glm::vec3 &start, glm::vec3 &end);
    
    void printTree();
private:
    struct BVHNode;
    struct StackItem;
    int numNodes;
    
    void buildBranch(int leftIndex, int rightIndex, BVHNode *node, int depth);
    std::shared_ptr<AABoundingBox> getAABBForSection(int start, int end);
    
    void printNode(BVHNode node, std::string indent, bool last);
    
    static bool compareByXAxis(const std::shared_ptr<GameObject> &a,
                               const std::shared_ptr<GameObject> &b);
    static bool compareByYAxis(const std::shared_ptr<GameObject> &a,
                               const std::shared_ptr<GameObject> &b);
    static bool compareByZAxis(const std::shared_ptr<GameObject> &a,
                               const std::shared_ptr<GameObject> &b);
    
    std::vector<std::shared_ptr<GameObject>> searchTreeWithRay(BVHNode node,
                                                               glm::vec3 &start,
                                                               glm::vec3 &end);
    std::vector<std::shared_ptr<GameObject>> objs;
    std::vector<BVHNode> nodeList;
};

#endif /* BVH_hpp */
