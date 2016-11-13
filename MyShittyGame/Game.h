#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <irrKlang.h>

#include "GameLevel.h"

class Game
{
public:
    irrklang::ISoundEngine* soundEngine;
    enum GameState {GAME_ACTIVE, GAME_MENU, GAME_WIN};
    GameState state;
    bool keys[1024];
    bool doorOpen;
    bool enemiesActive;
    GLuint width;
    GLuint height;
    std::vector<GameLevel> levels;
    std::vector<std::string> levelNames;
    GLuint level;

    Game(GLuint width, GLuint height);
    ~Game();

    void init();
    void processInput(GLfloat dt);
    void updateEnemies(GLfloat dt);
    void closeDoor();
    void openDoor();
    void closeExit();
    void openExit();
    void checkDoor();
    void checkExit();
    void doNextLevel();
    void update(GLfloat dt);
    void render();
    void checkCollisions();

};

#endif