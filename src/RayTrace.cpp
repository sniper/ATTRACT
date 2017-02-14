//
//  RayTrace.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/2/17.
//
//

#include <iostream>

#include "RayTrace.hpp"
#include "GameObject.hpp"
#include "Calculations.hpp"

using namespace std;
using namespace glm;

shared_ptr<GameObject> RayTrace::rayTrace(const vec3 &startLoc,
                                          const vec3 &endLoc,
                                          const vector<shared_ptr<GameObject>> &objList)
{
    shared_ptr<GameObject> closestObj, currObj;
    vec3 CPOI, currIntersection;
    cout << "Obj list in ray trace: " + to_string(objList.size()) << endl;
    for (unsigned int i = 0; i < objList.size(); i++) {
        currObj = objList.at(i);
        if (currObj->isIntersectingWithLineSegment(startLoc, endLoc, &currIntersection)) {
            // Update the closest object if:
            // (1) It is the first object found, OR
            // (2) It is closer than the previous CPOI.
            if (!closestObj || Calculations::findDistance(currIntersection, startLoc) <
                Calculations::findDistance(CPOI, startLoc)) {
                closestObj = currObj;
                CPOI = vec3(currIntersection.x, currIntersection.y, currIntersection.z);
            }
        }
    }
    
    return closestObj;
}
