//
//  BoundingSphere.hpp
//  Lab1
//
//  Created by Cody Rhoads on 1/16/17.
//
//

#ifndef BoundingSphere_hpp
#define BoundingSphere_hpp

#include <memory>
#include <glm/gtc/type_ptr.hpp>

class AABoundingBox;

class BoundingSphere
{
public:
    BoundingSphere();
    BoundingSphere(const glm::vec3 &center, float radius);
    virtual ~BoundingSphere();
    
    bool isCollidingWithSphere(const std::shared_ptr<BoundingSphere> sphere);
    bool isCollidingWithAABox(const std::shared_ptr<AABoundingBox> box);
    bool isPointInsideSphere(const glm::vec3 point);
    
    void updateCenter(const glm::vec3 &newCenter) {center = newCenter;}
    void updateRadius(float newRadius) {radius = newRadius;}
    glm::vec3 getCenter() const {return center;}
    float getRadius() const {return radius;}
private:
    glm::vec3 center;
    float radius;
};

#endif /* BoundingSphere_hpp */
