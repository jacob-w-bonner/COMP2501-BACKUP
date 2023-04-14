#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, bool invulnerable_=false);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            // Cooldown timer for player shooting
            float cooldown;

    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
