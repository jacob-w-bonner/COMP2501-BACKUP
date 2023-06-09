#include "bullet_game_object.h"

namespace game {

/*
    BulletGameObject inherits from GameObject
    It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

BulletGameObject::BulletGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, bool invulnerable_)
: GameObject(position, geom, shader, texture) {
    hostile_ = false;
    bulletEnd = 0;
    start = position;
}

// Update function for moving the player object around
void BulletGameObject::Update(double delta_time) {

    // Special player updates go here

    // Call the parent's update method to move the object in standard way, if desired
    GameObject::Update(delta_time);

}

} // namespace game