//
//  Calculations.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/7/17.
//
//

#ifndef Calculations_hpp
#define Calculations_hpp

#include <glm/gtc/type_ptr.hpp>

class Calculations
{
public:
    static float findDistance(const glm::vec3 &p1, const glm::vec3 &p2);
    static float randFloat(const float l, const float h);
};

#endif /* Calculations_hpp */
