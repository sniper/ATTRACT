//
//  KDTree.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/9/17.
//
//

#include "KDTree.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include "KDTree.hpp"
#include "GameObject.hpp"
#include "AABoundingBox.hpp"
#include "Cuboid.hpp"

using namespace std;
using namespace glm;

struct KDTree::KDNode
{
    int axis; // the axis that this node splits on.
    shared_ptr<GameObject> obj; // the object at this split.
    float split; // the value that splits between the left and right branch.
    shared_ptr<KDNode> left; // left branch.
    shared_ptr<KDNode> right; // right branch.
    
    void printTree(string indent, bool last)
    {
        cout << indent;
        
        if (last) {
            cout << "\\-";
            indent += "  ";
        }
        else {
            cout << "|-";
            indent += "| ";
        }
        
        string xCoord = to_string(obj->getPosition().x);
        xCoord.erase(xCoord.find_last_not_of('0') + 1, string::npos);
        if (xCoord[xCoord.size()-1] == '.') {
            xCoord.append("0");
        }
        string yCoord = to_string(obj->getPosition().y);
        yCoord.erase(yCoord.find_last_not_of('0') + 1, string::npos);
        if (yCoord[yCoord.size()-1] == '.') {
            yCoord.append("0");
        }
        string zCoord = to_string(obj->getPosition().z);
        zCoord.erase(zCoord.find_last_not_of('0') + 1, string::npos);
        if (zCoord[zCoord.size()-1] == '.') {
            zCoord.append("0");
        }
        
        cout << "(" + xCoord + ", " + yCoord + ", " + zCoord + ")" << endl;
        
        if (left) {
            // If there isn't a right branch, then the left is the last branch.
            if (!right) {
                left->printTree(indent, true);
            }
            else {
                left->printTree(indent, false);
            }
        }
        if (right) {
            right->printTree(indent, true);
        }
    }
};

KDTree::KDTree(const vector<shared_ptr<GameObject>> &objects) :
root(nullptr)
{
    // Create our own list of objects that is mutable. We don't want to edit the
    // provided list.
    vector<shared_ptr<GameObject>> objs = vector<shared_ptr<GameObject>>(objects);
    
    // Don't build the tree if it is empty.
    if (objs.size()) {
        root = createBranch(0, objs);
    }
}

KDTree::~KDTree()
{
    
}

vector<shared_ptr<GameObject>> KDTree::findObjectsNearPoint(vec3 point)
{
    if (root) {
        return searchTree(root, point);
    }
    
    return vector<shared_ptr<GameObject>>();
}

vector<shared_ptr<GameObject>> KDTree::findObjectsIntersectedByRay(vec3 start,
                                                                   vec3 end)
{
    if (root) {
        return searchTreeWithRay(root, start, end);
    }
    
    return vector<shared_ptr<GameObject>>();
}

void KDTree::printTree()
{
    if (root) {
        root->printTree("", true);
    }
}

shared_ptr<KDTree::KDNode> KDTree::createBranch(int axis,
                                                vector<shared_ptr<GameObject>> &objects)
{
    vector<shared_ptr<GameObject>> leftBranch, rightBranch;
    float median;
    shared_ptr<KDNode> newNode = make_shared<KDNode>();
    
    // If there is only one object left, then it is a leaf node.
    if (objects.size() == 1) {
        newNode->axis = -1;
        newNode->split = -1;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->obj = objects.at(0);
        return newNode;
    }
    
    // Sort the list by axis.
    if (axis == 0) {
//        cout << "Sort by x" << endl;
        sort(objects.begin(), objects.end(), &KDTree::compareByXAxis);
    }
    else if (axis == 1) {
//        cout << "Sort by y" << endl;
        sort(objects.begin(), objects.end(), &KDTree::compareByYAxis);
    }
    else {
//        cout << "Sort by z" << endl;
        sort(objects.begin(), objects.end(), &KDTree::compareByZAxis);
    }
    
//    cout << "Positions are: " << endl;
//    for (int i = 0; i < objects.size(); i++) {
//        cout << to_string(objects.at(i)->getPosition()[0])
//        + " " + to_string(objects.at(i)->getPosition()[1])
//        + " " + to_string(objects.at(i)->getPosition()[2]) << endl;
//    }
    
    // Get the median.
    median = objects.at(objects.size()/2)->getPosition()[axis];
//    cout << "Median: " + to_string(median) << endl;
    
    for (int i = 0; i < objects.size(); i++) {
        shared_ptr<GameObject> currObj = objects.at(i);
        if (i != objects.size()/2) {
            if (currObj->isCuboid()) {
                // Checking to see if the min bounding box of the object is less
                // than the median; if so, include on left branch.
                if (dynamic_pointer_cast<Cuboid>(currObj)->getMins()[axis] <= median) {
                    leftBranch.push_back(currObj);
                }
                // Checking to see if the max bounding box of the object exceeds
                // the median; if so, include on right branch.
                if (dynamic_pointer_cast<Cuboid>(currObj)->getMaxes()[axis] > median) {
                    rightBranch.push_back(currObj);
                }
            }
        }
    }
    
    // Add the new KDNode for the KDTree
    newNode->axis = axis;
    newNode->split = median;
    newNode->obj = objects.at(objects.size()/2);
    
    if (!leftBranch.size()) {
        newNode->left = nullptr;
    }
    else {
        newNode->left = createBranch((axis + 1) % 3, leftBranch);;
    }
    
    if (!rightBranch.size()) {
        newNode->right = nullptr;
    }
    else {
        newNode->right = createBranch((axis + 1) % 3, rightBranch);
    }
    
    return newNode;
}

vector<shared_ptr<GameObject>> KDTree::searchTree(shared_ptr<KDNode> node,
                                                  vec3 point)
{
    vector<shared_ptr<GameObject>> objs = vector<shared_ptr<GameObject>>();
    
    if (node->axis == -1) {
        objs.push_back(node->obj);
        return objs;
    }
    
    if (point[node->axis] <= node->split) {
        if (node->left) {
            objs = searchTree(node->left, point);
        }
        objs.push_back(node->obj);
        return objs;
    }
    else {
        if (node->right) {
            objs = searchTree(node->right, point);
        }
        objs.push_back(node->obj);
        return objs;
    }
}

vector<shared_ptr<GameObject>> KDTree::searchTreeWithRay(shared_ptr<KDNode> node,
                                                         vec3 start, vec3 end)
{
    float t, lineX, lineY, lineZ;
    vec3 segmentDivide;
    vector<shared_ptr<GameObject>> objs = vector<shared_ptr<GameObject>>();
    vector<shared_ptr<GameObject>> left = vector<shared_ptr<GameObject>>();
    vector<shared_ptr<GameObject>> right = vector<shared_ptr<GameObject>>();
    
    if (node->axis == -1) {
        objs.push_back(node->obj);
        return objs;
    }
    
    if (node->axis == 0) {
        t = (node->split - start.x)/(end.x - start.x);
        
        // Get the y and z values at point of intersection.
        lineY = start.y + (end.y - start.y) * t;
        lineZ = start.z + (end.z - start.z) * t;
        segmentDivide = vec3(node->split, lineY, lineZ);

        // Check if point is within line segment.
        // If it is, then check both children.
        if (lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y) &&
            lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
            if (start.x <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, segmentDivide);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, segmentDivide, end);
                }
            }
            else {
                if (node->left) {
                    left = searchTreeWithRay(node->left, segmentDivide, end);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, segmentDivide);
                }
            }
        }
        // Else, check only the side that the segment is in.
        else {
            if (start.x <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, end);
                }
            }
            else {
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, end);
                }
            }
        }
    }
    else if (node->axis == 1) {
        t = (node->split - start.y)/(end.y - start.y);
        
        // Get the x and z values at point of intersection.
        lineX = start.x + (end.x - start.x) * t;
        lineZ = start.z + (end.z - start.z) * t;
        segmentDivide = vec3(lineX, node->split, lineZ);
        
        // Check if point is within line segment.
        // If it is, then check both children.
        if (lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
            lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
            if (start.y <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, segmentDivide);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, segmentDivide, end);
                }
            }
            else {
                if (node->left) {
                    left = searchTreeWithRay(node->left, segmentDivide, end);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, segmentDivide);
                }
            }
        }
        // Else, check only the side that the segment is in.
        else {
            if (start.x <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, end);
                }
            }
            else {
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, end);
                }
            }
        }
    }
    else {
        t = (node->split - start.z)/(end.z - start.z);
        
        // Get the x and y values at point of intersection.
        lineX = start.x + (end.x - start.x) * t;
        lineY = start.y + (end.y - start.y) * t;
        segmentDivide = vec3(lineX, lineY, node->split);
        
        // Check if point is within line segment.
        // If it is, then check both children.
        if (lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
            lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y)) {
            if (start.z <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, segmentDivide);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, segmentDivide, end);
                }
            }
            else {
                if (node->left) {
                    left = searchTreeWithRay(node->left, segmentDivide, end);
                }
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, segmentDivide);
                }
            }
        }
        // Else, check only the side that the segment is in.
        else {
            if (start.z <= node->split) {
                if (node->left) {
                    left = searchTreeWithRay(node->left, start, end);
                }
            }
            else {
                if (node->right) {
                    right = searchTreeWithRay(node->right, start, end);
                }
            }
        }
    }
    
    objs.reserve(left.size() + right.size());
    objs.insert(objs.end(), left.begin(), left.end());
    objs.insert(objs.end(), right.begin(), right.end());
    objs.push_back(node->obj);

    return objs;
}


bool KDTree::compareByXAxis(const shared_ptr<GameObject> &a,
                            const shared_ptr<GameObject> &b)
{
    return a->getPosition().x < b->getPosition().x;
}

bool KDTree::compareByYAxis(const shared_ptr<GameObject> &a,
                            const shared_ptr<GameObject> &b)
{
    return a->getPosition().y < b->getPosition().y;
}

bool KDTree::compareByZAxis(const shared_ptr<GameObject> &a,
                            const shared_ptr<GameObject> &b)
{
    return a->getPosition().z < b->getPosition().z;
}
