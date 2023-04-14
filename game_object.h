#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            // Getters
            inline glm::vec3& GetPosition(void) { return position_; }
            inline glm::vec3& GetScale(void) { return scale_; }
            inline glm::vec3& GetVelocity(void) { return velocity_; }
            inline glm::vec3& GetRotation(void) { return roPoint; }

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(glm::vec3 scale) { scale_ = scale; }
            inline void SetRotation(const glm::vec3& r) { roPoint = r; }
            inline void SetVelocity(const glm::vec3& velocity) { velocity_ = velocity; }
            inline void SetShader(Shader *shader) { shader_ = shader; }

            // Object hostility value
            bool hostile_ = false;

            // Stores the state of whether or not object is tracking player
            bool state;

            // Stores the state about whether or not the object is alive or not
            bool deceased;

            // The player location
            glm::vec3 player;

            // The previous rotation angle
            float rotAngle;

            // Getter for the rotation matrix and angle
            glm::mat4 GetRotate();
            float GetAngle();

            // Setter for the rotation matrix and angle
            void SetRotate(glm::mat4);
            void SetAngle(float);

            // Number of tiles
            int tileNum;

            // A scalar for acceleration
            float accel;

            // Keep track of despawn time
            double despawn_;

            // Keep track of shooting cooldown
            float coolDown;

            // Stores angle of rotation
            float angle_;

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec3 scale_;
            glm::vec3 velocity_;
            // TODO: Add more transformation variables

            // The rotation matrix
            glm::mat4 rotate_;

            // Stores rotation point
            glm::vec3 roPoint;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
