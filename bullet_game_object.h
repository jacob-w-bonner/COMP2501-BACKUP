#ifndef BULLET_GAME_OBJECT_H_
#define BULLET_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class BulletGameObject : public GameObject {

        public:
            BulletGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, bool invulnerable_=false);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            // The timer for deleting the bullet
            float bulletEnd;

            // Starting point
            glm::vec3 start;

    }; // class BulletGameObject

} // namespace game

#endif BULLET_GAME_OBJECT_H_