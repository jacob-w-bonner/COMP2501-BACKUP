#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <time.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>
#include <math.h>

#include <path_config.h>
#include <glm/gtx/string_cast.hpp>

#include "sprite.h"
#include "shader.h"
#include "player_game_object.h"
#include "enemy_game_object.h"
#include "collectible_game_object.h"
#include "bullet_game_object.h"
#include "particles.h"
#include "particle_system.h"
#include "blade_game_object.h"
#include "game.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "Assignment 4";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set window to not resizable
    // Required or else the calculation to get cursor pos to screenspace will be incorrect
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize dead enemy shader
    dead_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/dead_sprite_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }
    for (int i = 0; i < enemies_.size(); i++) {
        delete enemies_[i];
    }
    for (int i = 0; i < bullets_.size(); i++) {
        delete bullets_[i];
    }
    for (int i = 0; i < particleVec_.size(); i++) {
        delete particleVec_[i];
    }
    for (int i = 0; i < exVec_.size(); i++) {
        delete exVec_[i];
    }

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setting the number of lives
    lives_ = 2;

    // Setting the loop break condition
    breakout_ = false;

    // Setting the number of items the player has picked up
    items_ = 0;

    // Setting that the user is not invulnerable to start
    invulnerable_ = false;

    // Setting the time for invulnerability
    invTime_ = 0;

    // Determining if the player is dead
    dead = false;

    // Setting up time for new enemy to spawn
    spawn = 7;

    // Setting up random number seed
    srand(time(NULL));

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    game_objects_.push_back(new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[0]));

    // Setup other objects
    enemies_.push_back(new EnemyGameObject(glm::vec3(-2.2f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    enemies_.push_back(new EnemyGameObject(glm::vec3(2.8f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(-3.5f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(3.5f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(0.0f, 3.5f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(-3.0f, -3.5f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    game_objects_.push_back(new CollectibleGameObject(glm::vec3(3.5f, -3.5f, 0.0f), sprite_, &sprite_shader_, tex_[6]));

    // Setting up the blade object
    GameObject* blade = new BladeGameObject(glm::vec3(0.0f, 0.0f, -1.0f), sprite_, &sprite_shader_, tex_[9], game_objects_[0]);
    blade->SetScale(glm::vec3(3.0f, 3.0f, 0.0f));
    game_objects_.push_back(blade);

    // Setting up demonstration black hole object
    GameObject* blackHole = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[7]);
    blackHole->SetScale(glm::vec3(10.0f, 3.0f, 1.0f));
    blackHole->SetRotate(glm::rotate(blackHole->GetRotate(), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    blackHole->SetAngle(blackHole->GetAngle() + glm::radians(45.0f));
    game_objects_.push_back(blackHole);

    // Setup background
    // In this specific implementation, the background is always the
    // last object
    GameObject *background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[3]);
    background->SetScale(glm::vec3(100.0f, 100.0f, 1.0f));
    background->tileNum = 10;
    game_objects_.push_back(background);
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    glGenTextures(NUM_TEXTURES, tex_);
    SetTexture(tex_[0], (resources_directory_g+std::string("/textures/body_01.png")).c_str());
    SetTexture(tex_[1], (resources_directory_g+std::string("/textures/body_02.png")).c_str());
    SetTexture(tex_[2], (resources_directory_g+std::string("/textures/body_03.png")).c_str());
    SetTexture(tex_[3], (resources_directory_g+std::string("/textures/stars.png")).c_str());
    SetTexture(tex_[4], (resources_directory_g+std::string("/textures/orb.png")).c_str());
    SetTexture(tex_[5], (resources_directory_g+std::string("/textures/explosion.png")).c_str());
    SetTexture(tex_[6], (resources_directory_g+std::string("/textures/item.png")).c_str());
    SetTexture(tex_[7], (resources_directory_g + std::string("/textures/Black_hole.png")).c_str());
    SetTexture(tex_[8], (resources_directory_g + std::string("/textures/bullet.png")).c_str());
    SetTexture(tex_[9], (resources_directory_g + std::string("/textures/blade.png")).c_str());
    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Clear background
        glClearColor(viewport_background_color_g.r,
                     viewport_background_color_g.g,
                     viewport_background_color_g.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view to zoom out, centered by on the player
        float camera_zoom = 0.25f;
        glm::mat4 view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update other events like input handling
        glfwPollEvents();

        // Update the game
        Update(view_matrix, delta_time);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Condition to end the game
        if (breakout_) {
            break;
        }

    }
}


void Game::Update(glm::mat4 view_matrix, double delta_time)
{

    // Updating the camera position
    if (lives_ < 0) {
        view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) * glm::translate(glm::mat4(1.0f), -1.0f * deadVec);
    }
    else {
        view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f)) * glm::translate(glm::mat4(1.0f), -1.0f * game_objects_[0]->GetPosition());
    }

    // Update time
    current_time_ += delta_time;

    // Handle user input
    if (lives_ >= 0) {
        Controls(delta_time);
    }

    // Checking to see if new enemy should spawn
    if (current_time_ > spawn) {
        spawn += 7;
        int subFac = rand() % 4;
        float xCoord = (rand() % 3 - subFac);
        float yCoord = (rand() % 3 - subFac);
        enemies_.push_back(new EnemyGameObject(glm::vec3(xCoord, yCoord, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    }

    // Update and render all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        // Updating the player's current position in other objects
        current_game_object->player = game_objects_[0]->GetPosition();

        // Updating shooting cooldown
        if (current_game_object->coolDown < current_time_ && current_game_object->coolDown != 0) {
            current_game_object->coolDown = 0;
        }

        // Update the current game object
        current_game_object->Update(delta_time);


        // Checking bullets
        for (int k = 0; k < bullets_.size(); k++) {
            // Grabbing bullet from vector
            BulletGameObject* bulObj = bullets_[k];

            // Deleting the bullets after a certain amount of time
            if (current_time_ > bulObj->bulletEnd) {
                bullets_.erase(bullets_.begin() + k);
                particleVec_.erase(particleVec_.begin() + k);
            }

            // Checking for collisions with enemies
            for (int j = 0; j < enemies_.size(); j++) {

                // Grabbing an enemy from the vector
                GameObject* enObj = enemies_[j];

                // This boolean determines whether or not there was a ray collision
                bool collide = false;

                // STEP 1

                // These vectors are start and end points for the ray so it is not too long
                glm::vec3 startPoint = bulObj->GetPosition();
                glm::vec3 endPoint = bulObj->GetPosition() +  glm::normalize(bulObj->GetVelocity());

                // Finding the distance between the circle and the start and end points
                float distStart = sqrt(pow((startPoint[0] - enObj->GetPosition()[0]), 2) + pow((startPoint[1] - enObj->GetPosition()[1]), 2));
                float distEnd = sqrt(pow((endPoint[0] - enObj->GetPosition()[0]), 2) + pow((endPoint[1] - enObj->GetPosition()[1]), 2));

                // Checking whether or not the start or end points of the ray are colliding with the enemy
                if ((distStart <= 0.5f || distEnd <= 0.5f) && enObj->deceased == false) {
                    collide = true;
                }

                // STEP 2

                // Representing the closest point on the ray as a vector
                glm::vec3 ray = endPoint - startPoint;
                float rayLen = glm::length(ray);
                ray = glm::normalize(ray);
                glm::vec3 vector = enObj->GetPosition() - startPoint;
                float direction = glm::dot(vector, ray);
                direction = glm::clamp<float>(direction, 0.0f, rayLen);
                glm::vec3 closest = startPoint + ray * direction;

                // Getting the distance from the ray start and end points and the previously derived point
                float firstDist = glm::length(closest - startPoint);
                float secondDist = glm::length(closest - endPoint);

                // Checking to see if the distances are equal to the line's length
                if (!(firstDist + secondDist >= rayLen - 0.1f && firstDist + secondDist <= rayLen + 0.1f) && !collide) {
                    continue;
                }

                // Getting the distance from the closest point on the line to the enemy and checking for collision
                float lineDist = glm::length(closest - enObj->GetPosition());
                if (lineDist <= 0.5f) {
                    collide = true;
                }

                // Dealing with detected collisions
                if (collide && bullets_.size() > 0 && enemies_.size() > 0 && !enObj->deceased) {
                    // Dealing with bullet and enemy collision
                    bullets_.erase(bullets_.begin() + k);
                    particleVec_.erase(particleVec_.begin() + k);
                    enObj->deceased = true;
                    enObj->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    enObj->SetShader(&dead_shader_);
                    enObj->despawn_ = current_time_ + 6;

                    // Setup particle system
                    Geometry* particles_ = new Particles(true);
                    particles_->CreateGeometry();
                    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], enObj, true);
                    particles->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
                    particles->despawn_ = current_time_ + 2.0f;
                    exVec_.push_back(particles);
                }
            }

        }

        // Checking enemies
        for (int k = 0; k < enemies_.size(); k++) {
            // Grabbing enemy from vector
            GameObject* enObj = enemies_[k];

            // Updating the player's position
            enObj->player = game_objects_[0]->GetPosition();

            // Handling the movement of the enemies
            if (enObj->state == false && dead == false && enObj->deceased == false) {
                // Patrolling (rotating) movement
                glm::vec3 tempPos = enObj->GetPosition();
                double xRot = (enObj->GetRotation()[0] + (tempPos[0] - enObj->GetRotation()[0]) * cos(0.1 * delta_time) - (tempPos[1] - enObj->GetRotation()[1]) * sin(0.1 * delta_time));
                double yRot = (enObj->GetRotation()[1] + (tempPos[1] - enObj->GetRotation()[1]) * cos(0.1 * delta_time) + (tempPos[0] - enObj->GetRotation()[0]) * sin(0.1 * delta_time));
                enObj->SetPosition(glm::vec3(xRot, yRot, 0.0));

                // Updating rotate value
                enObj->SetRotate(glm::mat4(1.0f));
                enObj->SetAngle(0.0f);
                glm::vec3 dirVec = enObj->GetPosition() - enObj->GetRotation();
                glm::vec3 axisVec = glm::vec3(1.0f, 0.0f, 0.0f);
                float dirLength = glm::length(dirVec);
                float axisLength = glm::length(axisVec);
                float theta = acos(glm::dot(axisVec, dirVec) / (axisLength * dirLength)) * 180 / 3.14159265358979323846;

                // Ensuring enemy turns in the right direction
                if (enObj->rotAngle > theta) {
                    enObj->rotAngle = theta;
                    theta *= -1.0f;
                }
                else {
                    enObj->rotAngle = theta;
                }
                enObj->SetRotate(glm::rotate(enObj->GetRotate(), glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f)));
                enObj->SetAngle(enObj->GetAngle() + glm::radians(theta));
                
            } else if (dead == false && enObj->deceased == false) {
                // Moving (vector) movement
                glm::vec3 dirVec = enObj->player - enObj->GetPosition();

                // Updating rotate value
                enObj->SetRotate(glm::mat4(1.0f));
                enObj->SetAngle(0.0f);
                glm::vec3 axisVec = glm::vec3(1.0f, 0.0f, 0.0f);
                float dirLength = glm::length(dirVec);
                float axisLength = glm::length(axisVec);
                float theta = acos(glm::dot(axisVec, dirVec) / (axisLength * dirLength)) * 180 / 3.14159265358979323846 + 90;

                // Ensuring the enemy rotates in the right direction
                if (enObj->player[1] > enObj->GetPosition()[1]) {
                    theta = 180 + theta;
                }
                else {
                    theta *= -1.0f;
                }
                enObj->SetRotate(glm::rotate(enObj->GetRotate(), glm::radians(theta), glm::vec3(0.0f, 0.0f, 1.0f)));
                enObj->SetAngle(enObj->GetAngle() + glm::radians(theta));

                // Applying velocity
                enObj->SetVelocity(0.15f * glm::normalize(dirVec));
            }

            // Update the current game object
            enObj->Update(delta_time);

            // Compute distance between the player and the enemy
            float distance = glm::length(enObj->GetPosition() - game_objects_[0]->GetPosition());

            // If distance reaches an upper threshold, the enemy begins to follow the player
            if (distance < 1.75f * current_game_object->GetScale()[0] - 0.2f && i == 0 && enObj->state == false) {
                enObj->state = true;
            }

            // If distance is below a lower threshold, we have a collision
            if (distance < current_game_object->GetScale()[0] - 0.2f && dead == false && i < game_objects_.size() - 3) {

                if (invulnerable_ == false && enObj->deceased == false) {

                    // Exploding collided enemy
                    enObj->deceased = true;
                    enObj->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    enObj->SetShader(&dead_shader_);

                    // Setup particle system
                    Geometry* particles_ = new Particles(true);
                    particles_->CreateGeometry();
                    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], enObj, true);
                    particles->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
                    particles->despawn_ = current_time_ + 2.0f;
                    exVec_.push_back(particles);

                    // Exploding the player
                    if (lives_ <= 0) {

                        // Setup particle system
                        Geometry* particles_ = new Particles(true);
                        particles_->CreateGeometry();
                        GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], game_objects_[0], true);
                        particles->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
                        particles->despawn_ = current_time_ + 2.0f;
                        exVec_.push_back(particles);

                        deadVec = game_objects_[0]->GetPosition();
                        game_objects_[0]->SetShader(&dead_shader_);
                        game_objects_[0]->deceased = true;
                        for (int l = 0; l < game_objects_.size(); l++) {
                            game_objects_[l]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                        }
                        for (int m = 0; m < enemies_.size(); m++) {
                            enemies_[m]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                        }
                        dead = true;
                        end_time_ = current_time_ + 3.0f;
                    }

                    // Subtracting player lives and setting explosion and despawn end times
                    lives_ -= 1;
                    enObj->despawn_ = current_time_ + 6;

                }

            }

            // Rendering the enemy object
            enObj->Render(view_matrix, current_time_);

            // Checking to see if the enemy should be despawned
            if (current_time_ > enObj->despawn_ && enObj->despawn_ > 0) {
                enemies_.erase(enemies_.begin() + k);
            }
        }

        // Check for collision with other game objects
        // Note the loop bounds: we avoid testing the last object since
        // it's the background covering the whole game world
        for (int j = i + 1; j < (game_objects_.size()-3); j++) {
            GameObject* other_game_object = game_objects_[j];

            // Compute distance between object i and object j
            float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());

            // If distance is below a lower threshold, we have a collision
            if (distance < current_game_object->GetScale()[0] - 0.2f ) {

                if (other_game_object->hostile_ == false) {

                    game_objects_.erase(game_objects_.begin() + j);
                    items_++;

                    if (items_ == 5) {
                        items_ = 0;
                        invulnerable_ = true;
                        SetTexture(tex_[0], (resources_directory_g + std::string("/textures/body_04.png")).c_str());
                        invTime_ = current_time_ + 10;
                    }

                }

            }
        }

        // Resetting the explosion at the proper time
        if (current_time_ >= end_time_ && end_time_ > 0) {

            // Ending the game upon player death
            if (lives_ < 0 && !breakout_) {
                std::cout << "Game Over" << std::endl;
                breakout_ = true;
            }
        }

        // Resetting the player at the proper time
        if (current_time_ >= invTime_ && invTime_ > 0) {
            SetTexture(tex_[0], (resources_directory_g + std::string("/textures/body_01.png")).c_str());
            invulnerable_ = false;
            invTime_ = 0;
        }

        // Render game object
        current_game_object->Render(view_matrix, current_time_);

        // Checking bullets and tail particles
        for (int k = 0; k < particleVec_.size(); k++) {

            // Grabbing bullet from vector
            BulletGameObject* bulObj = bullets_[k];

            // Update the current game object
            bulObj->Update(delta_time);

            // Rendering the enemy object
            bulObj->Render(view_matrix, current_time_);

            // Grabbing particle system from vector
            GameObject* parObj = particleVec_[k];

            // Update the current game object
            parObj->Update(delta_time);

            // Rendering the particle system object
            parObj->Render(view_matrix, current_time_);
        }

        // Checking the explosion particles
        for (int k = 0; k < exVec_.size(); k++) {
            // Grabbing particle system from vector
            GameObject* parObj = exVec_[k];

            // Update the current game object
            parObj->Update(delta_time);

            // Rendering the particle system object
            parObj->Render(view_matrix, current_time_);

            // Resetting the explosion at the proper time
            if (parObj->despawn_ < current_time_) {
                exVec_.erase(exVec_.begin() + k);
            }
        }
    }
}

void Game::Controls(double delta_time)
{
    // Get player game object
    GameObject *player = game_objects_[0];
    // Get current position
    glm::vec3 curpos = player->GetPosition();
    // Set standard forward and right directions
    glm::vec3 dir = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 right = glm::vec3(1.0, 0.0, 0.0);
    // Adjust motion increment based on a given speed
    float speed = 2.5;
    float motion_increment = speed*delta_time;

    // Grabbing a 3x3 part of the rotation matrix
    glm::mat3 rot = glm::mat3(player->GetRotate()[0][0], player->GetRotate()[0][1], player->GetRotate()[0][2],
        player->GetRotate()[1][0], player->GetRotate()[1][1], player->GetRotate()[1][2],
        player->GetRotate()[2][0], player->GetRotate()[2][1], player->GetRotate()[2][2]);

    // Check for player input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        //player->SetPosition(curpos + motion_increment * rot * dir);
        // Setting velocity
        player->SetVelocity(rot * dir * player->accel);

        // Increasing the player's acceleration
        if (player->accel < 5.0f) {
            player->accel += 0.05f;
            // Additional error checking
            if (player->accel > 5.0f) {
                player->accel = 5.0f;
            }
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        //player->SetPosition(curpos - motion_increment * rot * dir);
        // Setting player velocity
        player->SetVelocity(rot * dir * player->accel);

        // Decreasing acceleration
        if (player->accel > 0.0f) {
            player->accel -= 0.05f;
            // Additional error checking
            if (player->accel < 0.0f) {
                player->accel = 0.0f;
            }
        }

    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        //player->SetPosition(curpos + motion_increment*right);
        // Setting the player's bearing and rotation
        player->SetRotate(glm::rotate(player->GetRotate(), glm::radians(-0.6f), glm::vec3(0.0f, 0.0f, 1.0f)));
        player->SetAngle(player->GetAngle() - glm::radians(0.6f));
        player->SetVelocity(rot * dir * player->accel);
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        //player->SetPosition(curpos - motion_increment*right);
        // Setting the player's bearing and rotation
        player->SetRotate(glm::rotate(player->GetRotate(), glm::radians(0.6f), glm::vec3(0.0f, 0.0f, 1.0f)));
        player->SetAngle(player->GetAngle() + glm::radians(0.6f));
        player->SetVelocity(rot * dir * player->accel);
    }
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // Checking to see if the cooldown permits shooting
        if (player->coolDown == 0) {
            // Making a new bullet object to be fired
            BulletGameObject* bullet = new BulletGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[8]);
            bullet->SetRotate(player->GetRotate());
            bullet->SetAngle(player->GetAngle());
            bullet->SetRotation(player->GetPosition());
            //bullet->SetPosition(player->GetPosition());

            glm::vec3 tempPos = player->GetPosition() - glm::vec3(0.5f, 0.0f, 0.0f);
            double xRot = (bullet->GetRotation()[0] + (tempPos[0] - bullet->GetRotation()[0]) * cos(player->GetAngle()) - (tempPos[1] - bullet->GetRotation()[1]) * sin(player->GetAngle()));
            double yRot = (bullet->GetRotation()[1] + (tempPos[1] - bullet->GetRotation()[1]) * cos(player->GetAngle()) + (tempPos[0] - bullet->GetRotation()[0]) * sin(player->GetAngle()));
            bullet->SetPosition(glm::vec3(xRot, yRot, 0.0));

            //bullet->SetPosition(player->GetPosition());
            bullet->SetVelocity(rot * dir * 100.0f);
            bullet->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
            bullet->bulletEnd = current_time_ + 3.0f;
            bullets_.push_back(bullet);
            
            // Setting the shooting cooldown
            player->coolDown = current_time_ + 1.0f;

            // Setup particle system
            Geometry* particles_ = new Particles(false);
            particles_->CreateGeometry();
            GameObject* particles = new ParticleSystem(glm::vec3(0.0f, -0.5f, 0.0f), particles_, &particle_shader_, tex_[4], bullet, false);
            particles->SetScale(glm::vec3(0.05f, 0.25f, 0.1f));
            particleVec_.push_back(particles);
        }
    }
}
       
/* For lifespan, make a timer in the game object if its a bullet the timer determines when it is deleted 
Use player start and end point for determining the ray use distance formula then vector projection*/


} // namespace game
