#ifndef BLADE_GAME_OBJECT_H_
#define BLADE_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class BladeGameObject : public GameObject {

    public:
        BladeGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* parent);

        void Update(double delta_time) override;

        void Render(glm::mat4 view_matrix, double current_time);

    private:
        GameObject* parent_;

    }; // class BladeGameObject

} // namespace game

#endif // BLADE_GAME_OBJECT_H_