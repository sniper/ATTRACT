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
#include <vector>

class BoundingSphere;

class AABoundingBox
{
public:
    AABoundingBox();
    AABoundingBox(const glm::vec3 position, const glm::vec3 halfExtents);
    virtual ~AABoundingBox();
    
    bool isCollidingWithSphere(const std::shared_ptr<BoundingSphere> sphere);
    bool isCollidingWithAABox(const std::shared_ptr<AABoundingBox> box);
    
    void setPosition(const glm::vec3 newPos) {position = newPos;}
    
    glm::vec3 getPosition() const {return position;}
    float getXHalfExtent() const {return halfExtents.x;}
    float getYHalfExtent() const {return halfExtents.y;}
    float getZHalfExtent() const {return halfExtents.z;}
    glm::vec3 getMins() const {return glm::vec3(position.x - halfExtents.x,
                                                position.y - halfExtents.y,
                                                position.z - halfExtents.z);}
    glm::vec3 getMaxes() const {return glm::vec3(position.x + halfExtents.x,
                                                 position.y + halfExtents.y,
                                                 position.z + halfExtents.z);}
    std::vector<glm::vec3> *getVertices();
private:
    glm::vec3 position;
    glm::vec3 halfExtents;  // How far the center is from the edge of the
                            // cuboid; i.e. the length of the x dimension is
                            // halfExtents.x * 2
};

#endif /* AABoundingBox_hpp */
