#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "bullet_game_object.h"
#include "game_object.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            // Shader for rendering dead  enemy sprites in the scene
            Shader dead_shader_;

            // References to textures
#define NUM_TEXTURES 10
            GLuint tex_[NUM_TEXTURES];

            // List of game objects
            std::vector<GameObject*> game_objects_;
            std::vector<GameObject*> enemies_;
            std::vector<BulletGameObject*> bullets_;
            std::vector<GameObject*> particleVec_;
            std::vector<GameObject*> exVec_;

            // Keep track of time
            double current_time_;

            // Keep track of an explosion disappearance time
            double end_time_;

            // Keep track of invulnerability duration
            double invTime_;

            // Keep track of player lives
            int lives_;

            // Tracks when the gameloop should end
            bool breakout_;

            // Tracks the number of items the player has picked up
            int items_;

            // Tracks if the player is dead
            bool dead;

            // Tracks when to spawn new enemy
            int spawn;

            // Tracks if player is invulnerable or not
            bool invulnerable_;

            // Setting up the dead player vector
            glm::vec3 deadVec;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void Controls(double delta_time);

            // Update the game based on user input and simulation
            void Update(glm::mat4 view_matrix, double delta_time);

    }; // class Game

} // namespace game

#endif // GAME_H_
