Changes

blade_game_object.h
-This new object extends a normal game object
-It represents a blade that moves through hierarchical transformations but also rotates independently
-Holds a pointer to the object parent

bullet_game_object.h
-This new object extends a normal game object
-It represents a projectile that the player can fire to kill enemies
-It holds a float to keep track of the bullet despawn timer and its starting position

game.h
-Increased the texture number from 7 to 10
-Added vectors to contain bullet objects as well as different kinds of particle

game_object.h
-Added value to keep track of the shooting cooldown

particle_system.h
-This new object extends a normal game object
-It represents a system of particles
-It holds a pointer to the parent object
-It holds floats that contain red, green, and blue values
-It holds a boolean value to keep track of whether or not the particle system represents an explosion or not

particles.h
-This new object extends geometry
-It represents an individual particle
-It holds a boolean value to determine whether or not the particle is part of an explosion

player_game_object.h
-Added a value to store a cooldown ending time for player shooting

blade_game_object.cpp
-This game object represents a propeller blade for the player object (the parent)
-Uses hierarchical transformation to travel with player but also rotates clockwise independently
-So long as the parent is not dead, the blade will continue to rotate
-Blade transformation matrix primarily determined by hierarchical transformation relative to the player (parent)

bullet_game_object.cpp
-This game object represents a bullet that the player can shoot to kill enemies
-Upon creation, the bullet despawn time and the bullet's starting position are set accordingly

game.cpp
-Changed window title to "Assignment 4"
-Added a new shader for particles
-Destructor now deletes all objects from necessary vectors
-Creates a blade object with the player as the parent
-Added new textures for the bullets and the blade
-Resets player shooting cooldown at the appropriate time
-Deletes bullet objects and associated particles when it is time for them to despawn (did not hit a target)
-Calculates a ray-circle collision to determine if the bullet will hit an enemy
-If bullet collision with an enemy is detected the bullet and its particle trail despawn
-If an enemy dies by colliding with a bullet or player an explosion particle system is created on top of the enemy
-If the player dies an explosion particle system is created on top of the player
-Player now rendered in grayscale upon death
-Fixed a bug from a previous assignment where the game over message would appear more than once after player death
-Updates and renders bullet objects and particle systems (both bullet tails and explosions)
-Explosion particle systems despawn after a few seconds
-Updated values to quicken acceleration/deceleration as well as quicker turning for the player
-Pressing spacebar now creates a bullet object with a particle system tail
-Bullet (and tail) are fired in the direction the player is facing, beginning at the player position
-Player shooting cooldown is initiated once spacebar is pressed

game_object.cpp
-Sets initial shooting cooldown to 0

particle_system.cpp
-This game object represents a system of particles
-Colours are determined upon creation based on whether or not particles are for a bullet tail or explosion
-Particle system is subject to hierarchical transformation from parent object
-If the particle system is an explosion, the colours get darker as the system persists
-RGB values are passed to the particle shader

particles.cpp
-This geometry represents a single particle
-Explosion boolean set based on whether or not the particle is part of the explosion
-The angles at which the particles travel as well as the blending is dependent on the explosion boolean

particle_vertex_shader.glsl
-This shader helps process particle vertex information for rendering
-Passes particle red, green, and blue values to the particle fragment shader
-Colour interpolation determined used phase

particle_fragment_shader.glsl
-This shader helps process particle information for fragments
-Colour RGB values are multiplied by the interpolation before being set


Assets
-Player and enemy sprites taken from https://zintoki.itch.io/space-breaker under CC license
-Explosion sprite taken from https://weisinx7.itch.io/fireball-explosion-sprites under CC license
-Collectible item sprite taken from https://alexs-assets.itch.io/16x16-rpg-item-pack under CC license
-Black hole sprite taken from https://helianthus-games.itch.io/pixel-art-planets under CC license

Developed on Windows