//
//  RayTrace.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/2/17.
//
//

#ifndef RayTracing_hpp
#define RayTracing_hpp

#include <memory>
#include <vector>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>

class GameObject;

class RayTrace
{
public:
    static std::shared_ptr<GameObject> rayTrace(const glm::vec3 &startLoc,
                                                const glm::vec3 &endLoc,
                                                const std::vector<std::shared_ptr<GameObject>> &objList);
};

#endif /* RayTrace_hpp */
