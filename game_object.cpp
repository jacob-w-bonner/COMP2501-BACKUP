#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture) 
{

    // Initialize all attributes
    position_ = position;
    scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
    velocity_ = glm::vec3(0.0f, 0.0f, 0.0f); // Starts out stationary
    geometry_ = geom;
    shader_ = shader;
    texture_ = texture;
    roPoint = position - glm::vec3(0.2f, 0.2f, 0.0f);
    state = false;
    rotate_ = glm::mat4(1.0f);
    rotAngle = 0.0f;
    tileNum = 1;
    accel = 0;
    deceased = false;
    despawn_ = 0;
    coolDown = 0;
    angle_ = 0.0f;
}


void GameObject::Update(double delta_time) {
    // Update object position with Euler integration
    position_ += velocity_ * ((float) delta_time);
}

// Getter for the rotation matrix
glm::mat4 GameObject::GetRotate() {
    return rotate_;
}

// Getter for the angle
float GameObject::GetAngle() {
    return angle_;
}

// Setter for the rotation matrix
void GameObject::SetRotate(glm::mat4 newRot) {
    rotate_ = newRot;
}

// Setter for the angle
void GameObject::SetAngle(float a) {
    angle_ = a;
}

void GameObject::Render(glm::mat4 view_matrix, double current_time){

    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), scale_);

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = translation_matrix * rotate_ * scaling_matrix;

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    shader_->SetUniform1i("tiles", tileNum);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}

} // namespace game
