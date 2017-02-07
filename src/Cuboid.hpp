//
//  Cuboid.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/6/17.
//
//

#ifndef Cuboid_hpp
#define Cuboid_hpp

#include "GameObject.hpp"

class Cuboid : public GameObject
{
public:
    Cuboid();
    Cuboid(const glm::vec3 &position, const glm::vec3 &direction,
           const glm::vec3 &halfExtents, const glm::vec3 &scale, float velocity,
           const std::shared_ptr<Shape> &shape,
           const std::shared_ptr<Material> &material, bool magnetic);
    virtual ~Cuboid();
    
    bool isCollidingWithBoundingSphere(const std::shared_ptr<BoundingSphere> &sphere);
    bool isCollidingWithBoundingBox(const std::shared_ptr<AABoundingBox> &box);
    bool isCollidingWithOtherObject(const std::shared_ptr<GameObject> &otherObj);
    
    bool isIntersectingWithLineSegment(const glm::vec3 &start,
                                       const glm::vec3 &end,
                                       glm::vec3 *closestPointOfIntersection);
    
    bool isMagnetic() const {return magnetic;}
    
    void update(float dt);
    std::vector<glm::vec3>* getAabbVertices();
protected:
    std::shared_ptr<AABoundingBox> boundingBox;
    bool magnetic;
};

#endif /* Cuboid_hpp */
