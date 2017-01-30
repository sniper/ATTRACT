//
//  AABoundingBox.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/29/17.
//
//

#ifndef AABoundingBox_hpp
#define AABoundingBox_hpp

#include <memory>
#include <glm/gtc/type_ptr.hpp>

class AABoundingBox
{
public:
    AABoundingBox();
    AABoundingBox(const glm::vec3 mins, const glm::vec3 maxes);
    virtual ~AABoundingBox();
    
    bool isColliding(const std::shared_ptr<AABoundingBox> otherBox);
    
    void setMinX(float x) {min.x = x;}
    void setMaxX(float x) {max.x = x;}
    void setMinY(float y) {min.y = y;}
    void setMaxY(float y) {max.y = y;}
    void setMinZ(float z) {min.z = z;}
    void setMaxZ(float z) {max.z = z;}
    
    float getMinX() {return min.x;}
    float getMaxX() {return max.x;}
    float getMinY() {return min.y;}
    float getMaxY() {return max.y;}
    float getMinZ() {return min.z;}
    float getMaxZ() {return max.z;}
private:
    glm::vec3 min, max;
};

#endif /* AABoundingBox_hpp */
