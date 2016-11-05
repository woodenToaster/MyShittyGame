#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "GameLevel.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

const glm::vec2 PLAYER_SIZE(20, 20);
const GLfloat PLAYER_VELOCITY(400.0f);

class Game
{
public:
    GameState state;
    bool keys[1024];
    GLuint width;
    GLuint height;
    std::vector<GameLevel> levels;
    GLuint level;
   
    Game(GLuint width, GLuint height);
    ~Game();

    void init();

    void processInput(GLfloat dt);
    void updateEnemies(GLfloat dt);
    void update(GLfloat dt);
    void render();
    void checkCollisions();
};

#endif