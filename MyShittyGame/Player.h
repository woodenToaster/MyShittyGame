#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Entity.h"

class Player : public Entity {
public:
    GLuint lives;
    bool alive;
    GLfloat velocity;

    Player(glm::vec2 playerPos, glm::vec2 playerSize);
    void moveUp(GLfloat dt);
    void moveDown(GLfloat dt, GLuint screenHeight);
    void moveLeft(GLfloat dt);
    void moveRight(GLfloat dt, GLuint screenWidth);

private:

};

#endif