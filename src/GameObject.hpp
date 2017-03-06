//
//  GameObject.hpp
//  Lab1
//
//  Created by Cody Rhoads on 5/19/16.
//
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shape;
class Program;
class Material;
class BoundingSphere;
class AABoundingBox;

class GameObject
{
public:
    GameObject();
    GameObject(const glm::vec3 &position, const glm::vec3 &direction,
               const glm::vec3 &scale, float velocity,
               const std::shared_ptr<Shape> &shape,
               const std::shared_ptr<Material> &material);
    virtual ~GameObject();
    
    virtual bool isCollidingWithBoundingSphere(const std::shared_ptr<BoundingSphere> &sphere);
    virtual bool isCollidingWithBoundingBox(const std::shared_ptr<AABoundingBox> &box);
    virtual bool isCollidingWithOtherObject(const std::shared_ptr<GameObject> &otherObj);
    void collidedWithPlayer(const std::shared_ptr<BoundingSphere> &playerSphere);
    
    virtual bool isIntersectingWithLineSegment(const glm::vec3 &start,
                                               const glm::vec3 &end,
                                               glm::vec3 *closestPointOfIntersection);
    
    virtual void draw(const std::shared_ptr<Program> &prog);
    virtual void update(float dt);
    void resolveCollision(bool collidedWithPlayer);
    
    virtual bool isCuboid() const {return false;}
    
    void setPosition(glm::vec3 pos);
    
    virtual bool isMagnetic() const {return false;}
    glm::vec3 getPosition() const {return position;}
    glm::vec3 getDirection() const {return direction;}
    glm::vec3 getScale() const {return scale;}
protected:
    glm::vec3 position;
    glm::vec3 oldPosition;
    glm::vec3 direction;
    glm::vec3 scale;
    float velocity;
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
};

#endif /* GameObject_hpp */
