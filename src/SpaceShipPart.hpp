//
//  SpaceShipPart.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/15/17.
//
//

#ifndef SpaceShipPart_hpp
#define SpaceShipPart_hpp

#include "Cuboid.hpp"

#include <glm/gtc/type_ptr.hpp>

class SpaceShipPart: public Cuboid
{
public:
    SpaceShipPart();
    SpaceShipPart(const glm::vec3 &position, const glm::vec3 &direction,
                  const glm::vec3 &halfExtents, const glm::vec3 &scale,
                  const std::shared_ptr<Shape> &shape,
                  const std::shared_ptr<Material> &material);
    virtual ~SpaceShipPart();
    
    void update(float dt);
    void draw(const std::shared_ptr<Program> &prog);
    void startWin();
    bool doneWinning();

protected:
    float yOffset;
    float currTime;
    bool collected;
    bool spinning;
    float spinTime;
};

#endif /* SpaceShipPart_hpp */
