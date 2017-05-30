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
    SpaceShipPart(const glm::vec3 &position, const glm::vec3 &direction,
                  const glm::vec3 &halfExtents, const glm::vec3 &scale,
                  const std::vector<std::shared_ptr<Shape>> &pieces,
                  const std::shared_ptr<Material> &material);
    
    void update(float dt);
    void draw(const std::shared_ptr<Program> &prog);
    void startWin(const glm::vec3 &camDir);
    bool doneWinning();

protected:
    std::vector<std::shared_ptr<Shape>> partPieces;
    std::vector<float> xOffsets;
    float yOffset;
    std::vector<float> zOffsets;
    float currTime;
    bool collected;
    bool spinning;
    bool expanding;
    float spinTime;
    float expandTime;
    std::vector<glm::vec3> expandDirs;
};

#endif /* SpaceShipPart_hpp */
