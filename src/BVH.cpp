//
//  BVH.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/20/17.
//
//

#include <iostream>
#include <string>
#include <algorithm>
#include <stack>
#include <glm/gtc/type_ptr.hpp>

#include "BVH.hpp"
#include "GameObject.hpp"
#include "AABoundingBox.hpp"
#include "Cuboid.hpp"
#include "Calculations.hpp"

#define THRESHOLD 2

using namespace std;
using namespace glm;

struct BVH::BVHNode
{
    shared_ptr<AABoundingBox> nodeBB;   // the bounding box for the node.
                                        // If a leaf, set to nullptr.
    bool leaf;
    unsigned int numObjs;
    unsigned int index; // if leaf, index is the first obj in objs vector.
                        // if !leaf, index is left child node.
};

struct BVH::StackItem
{
    BVHNode *node;
    float dist;
};

BVH::BVH(const vector<shared_ptr<GameObject>> &objects)
{
    buildTree(objects);
}

BVH::~BVH()
{
    
}

void BVH::buildTree(const vector<shared_ptr<GameObject>> &objects)
{
    BVHNode root;
    
    // Copy the objects so that we can manipulate order.
    objs = vector<shared_ptr<GameObject>>(objects);
    nodeList = vector<BVHNode>();
    
    root.nodeBB = getAABBForSection(0, objs.size());
    
    if (root.nodeBB) {
        nodeList.insert(nodeList.begin(), root);
        buildBranch(0, objs.size(), &nodeList.at(0), 0);
    }
}

shared_ptr<GameObject> BVH::findClosestHitObject(vec3 &start, vec3 &end)
{
    BVHNode curNode = nodeList.at(0);
    shared_ptr<GameObject> closestHit = nullptr;
    float closestDist = numeric_limits<float>::max();
    stack<StackItem> nodeStack;
    StackItem si;
    
    // If the node list is empty or if the ray isn't intersecting the world AABB,
    // then there are no objects intersected by the ray.
    if (!nodeList.size() || !curNode.nodeBB->isIntersectingWithRay(start, end,
                                                                   nullptr)) {
        return nullptr;
    }
    
    while (1) {
        if (!curNode.leaf) {
            vec3 leftIntersect, rightIntersect;
            
            bool leftHit = nodeList.at(curNode.index).nodeBB->isIntersectingWithRay(start, end,
                                                                                    &leftIntersect);
            bool rightHit = nodeList.at(curNode.index+1).nodeBB->isIntersectingWithRay(start, end,
                                                                                       &rightIntersect);
            
            if (leftHit && rightHit) {
                float leftDist = Calculations::findDistance(start, leftIntersect);
                float rightDist = Calculations::findDistance(start, rightIntersect);
                
                // If the left node is closer, explore that node first. Store the
                // farther node (right node) on the stack to check later.
                if (leftDist < rightDist) {
                    si.node = &nodeList.at(curNode.index+1);
                    si.dist = rightDist;
                    nodeStack.push(si);
                    curNode = nodeList.at(curNode.index);
                    continue;
                }
                // If the right node is closer, explore that node first. Store the
                // farther node (left node) on the stack to check later.
                else {
                    si.node = &nodeList.at(curNode.index);
                    si.dist = leftDist;
                    nodeStack.push(si);
                    curNode = nodeList.at(curNode.index+1);
                    continue;
                }
            }
            else if (leftHit) {
                curNode = nodeList.at(curNode.index);
                continue;
            }
            else if (rightHit) {
                curNode = nodeList.at(curNode.index+1);
                continue;
            }
        }
        else {
            // Testing every object contained in the leaf node for an intersection.
            // Keep track of the intersection if it is the closest so far.
            for (int i = curNode.index; i < curNode.index + curNode.numObjs; i++) {
                vec3 tempIntersect;
                float tempDist;
                if (objs.at(i)->isIntersectingWithLineSegment(start, end,
                                                              &tempIntersect)) {
                    tempDist = Calculations::findDistance(start, tempIntersect);
                    if (tempDist < closestDist) {
                        closestHit = objs.at(i);
                        closestDist = tempDist;
                    }
                }
            }
        }
        
        /* TODO: Try to make this work: it's supposed to prune the stack if
         the ray intersects that AABB at a farther distance than the current 
         closest distance. For some reason it's messing everything up. */
//        while (!nodeStack.empty() && nodeStack.top().dist > closestDist) {
//            nodeStack.pop();
//        }
        
        if (nodeStack.empty()) {
            return closestHit;
        }
        else {
            curNode = *(nodeStack.top().node);
            nodeStack.pop();
        }
    }
}

void BVH::buildBranch(int leftIndex, int rightIndex, BVHNode *node, int depth)
{
    int axis, splitIndex;
    vec3 mins, maxes;
    BVHNode left, right;
    
    if (rightIndex-leftIndex <= THRESHOLD) {
        node->leaf = true;
        node->index = leftIndex;
        node->numObjs = rightIndex-leftIndex;
    }
    else {
        // Find the largest dimension.
        mins = node->nodeBB->getMins();
        maxes = node->nodeBB->getMaxes();
        if (maxes.x - mins.x > maxes.y - mins.y) {
            axis = 0;
        }
        else {
            axis = 1;
        }
        if (maxes[axis] - mins[axis] < maxes.z - mins.z) {
            axis = 2;
        }
        
        // Sort by the largest dimension.
        if (axis == 0) {
            sort(objs.begin()+leftIndex, objs.begin()+rightIndex, &BVH::compareByXAxis);
        }
        else if (axis == 1) {
            sort(objs.begin()+leftIndex, objs.begin()+rightIndex, &BVH::compareByYAxis);
        }
        else {
            sort(objs.begin()+leftIndex, objs.begin()+rightIndex, &BVH::compareByZAxis);
        }
        
        // Find the split between the left and right branch.
        splitIndex = leftIndex + (rightIndex-leftIndex)/2;
        
        // Initialize current node as a non-leaf.
        node->index = nodeList.size();
        node->leaf = false;
        node->numObjs = 0;
        
        // Get AABBs for the children nodes.
        left.nodeBB = getAABBForSection(leftIndex, splitIndex);
        right.nodeBB = getAABBForSection(splitIndex, rightIndex);
        // Push left and right into nodeList
//        nodeList.insert(nodeList.begin()+node->index, left);
//        nodeList.insert(nodeList.begin()+node->index+1, right);
        nodeList.push_back(left);
        nodeList.push_back(right);
        
        // Build for left and right nodes.
        try {
            buildBranch(leftIndex, splitIndex, &nodeList.at(node->index), depth+1);
            buildBranch(splitIndex, rightIndex, &nodeList.at(node->index + 1), depth+1);
        }
        catch (int e) {
            cerr << "Caught out of bounds error; rebuilding tree" << endl;
            buildTree(objs);
        }
    }
}

shared_ptr<AABoundingBox> BVH::getAABBForSection(int start, int end)
{
    float minX, minY, minZ, maxX, maxY, maxZ;
    vec3 mins, maxes;
    
    // Don't calculate if the list is empty.
    if (objs.size()) {
        if (objs.at(start)->isCuboid()) {
            mins = dynamic_pointer_cast<Cuboid>(objs.at(start))->getMins();
            maxes = dynamic_pointer_cast<Cuboid>(objs.at(start))->getMaxes();
            minX = mins.x;
            minY = mins.y;
            minZ = mins.z;
            maxX = maxes.x;
            maxY = maxes.y;
            maxZ = maxes.z;
            
            for (int i = start+1; i < end; i++) {
                if (objs.at(i)->isCuboid()) {
                    mins = dynamic_pointer_cast<Cuboid>(objs.at(i))->getMins();
                    maxes = dynamic_pointer_cast<Cuboid>(objs.at(i))->getMaxes();
                    minX = std::min(minX, mins.x);
                    minY = std::min(minY, mins.y);
                    minZ = std::min(minZ, mins.z);
                    maxX = std::max(maxX, maxes.x);
                    maxY = std::max(maxY, maxes.y);
                    maxZ = std::max(maxZ, maxes.z);
                }
            }
        }
        else {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
    
    return make_shared<AABoundingBox>(vec3((minX + maxX)/2,
                                           (minY + maxY)/2,
                                           (minZ + maxZ)/2), // position
                                      vec3((maxX - minX)/2,
                                           (maxY - minY)/2,
                                           (maxZ - minZ)/2)); // half-extents
}

void BVH::printTree()
{
    if (nodeList.size()) {
        printNode(nodeList.at(0), "", true);
    }
}

void BVH::printNode(BVHNode node, string indent, bool last)
{
    string xCoord, yCoord, zCoord;
    
    cout << indent;
    
    if (last) {
        cout << "\\-";
        indent += "  ";
    }
    else {
        cout << "|-";
        indent += "| ";
    }
    
    cout << "Node" << endl;
    
    if (node.leaf) {
        for (int i = 0; i < node.numObjs; i++) {
            cout << indent;
            
            if (i == node.numObjs - 1) {
                cout << "\\-";
            }
            else {
                cout << "|-";
            }
            
            xCoord = to_string(objs.at(node.index + i)->getPosition().x);
            xCoord.erase(xCoord.find_last_not_of('0') + 1, string::npos);
            if (xCoord[xCoord.size()-1] == '.') {
                xCoord.append("0");
            }
            yCoord = to_string(objs.at(node.index + i)->getPosition().y);
            yCoord.erase(yCoord.find_last_not_of('0') + 1, string::npos);
            if (yCoord[yCoord.size()-1] == '.') {
                yCoord.append("0");
            }
            zCoord = to_string(objs.at(node.index + i)->getPosition().z);
            zCoord.erase(zCoord.find_last_not_of('0') + 1, string::npos);
            if (zCoord[zCoord.size()-1] == '.') {
                zCoord.append("0");
            }
            
            cout << "(" + xCoord + ", " + yCoord + ", " + zCoord + ")" << endl;
        }
    }
    else {
        printNode(nodeList.at(node.index), indent, false);
        printNode(nodeList.at(node.index+1), indent, true);
    }
}

bool BVH::compareByXAxis(const shared_ptr<GameObject> &a,
                         const shared_ptr<GameObject> &b)
{
    return a->getPosition().x < b->getPosition().x;
}

bool BVH::compareByYAxis(const shared_ptr<GameObject> &a,
                         const shared_ptr<GameObject> &b)
{
    return a->getPosition().y < b->getPosition().y;
}

bool BVH::compareByZAxis(const shared_ptr<GameObject> &a,
                         const shared_ptr<GameObject> &b)
{
    return a->getPosition().z < b->getPosition().z;
}
