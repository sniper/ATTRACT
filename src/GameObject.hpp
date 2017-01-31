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

class GameObject
{
public:
    GameObject();
    GameObject(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &scale,
               float velocity,
               const std::shared_ptr<BoundingSphere> &sphere,
               const std::shared_ptr<Shape> &shape,
               const std::shared_ptr<Material> &material);
    virtual ~GameObject();
    
    bool isCollidingWithBoundingSphere(const std::shared_ptr<BoundingSphere> &otherSphere);
    bool isCollidingWithOtherObject(const std::shared_ptr<GameObject> &otherObj);
    void collidedWithPlayer(const std::shared_ptr<BoundingSphere> &playerSphere);
    void draw(const std::shared_ptr<Program> &prog);
    void update(float dt);
    void resolveCollision(bool collidedWithPlayer);
    
    bool isCollected() const {return collected;}
    glm::vec3 getPosition() const {return position;}
    void setPosition(glm::vec3 pos);
    glm::vec3 getDirection() const {return direction;}
private:
    bool collected;
    glm::vec3 position;
    glm::vec3 oldPosition;
    glm::vec3 direction;
    float velocity;
    glm::vec3 scale;
    std::shared_ptr<BoundingSphere> boundingSphere;
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
};

#endif /* GameObject_hpp */
