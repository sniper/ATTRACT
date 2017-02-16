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

class Shape;
class Program;
class Material;

class GameObject
{
public:
    GameObject();
    GameObject(const glm::vec3 &position, const glm::vec3 &scale, const glm::vec3 &rotation,
               const std::shared_ptr<Shape> &shape,
               const bool magnetic, const bool deadly, const bool spawnPoint, const bool collectable);
    virtual ~GameObject();

    std::shared_ptr<Material> createMaterial(bool magnetic, bool deadly, bool spawnPoint, bool collectable);
    void draw(const std::shared_ptr<Program> &prog);

    glm::vec3 getPosition() const {return position;}
    void setPosition(glm::vec3 pos) {position = pos;}
    glm::vec3 getScale() const {return scale;}
    void setScale(glm::vec3 s) {scale = s;}
    glm::vec3 getRotation() const {return rotation;}
    void setRotation(glm::vec3 r) {rotation = r;}
    std::string toString();
    void setSelected(bool s) {selected = s;}
    bool getMagnetic() const {return magnetic;}
    void setMagnetic(bool mag);
    bool getDeadly() const {return deadly;}
    void setDeadly(bool dead);
private:
    glm::vec3 position, scale, rotation;
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
    bool magnetic, deadly, spawnPoint, collectable, selected;
};

#endif /* GameObject_hpp */
