#ifndef ENEMY_H
#define ENEMY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Entity.h"

const glm::vec2 ENEMY_SIZE(20, 20);
const GLfloat ENEMY_VELOCITY(500.0f);

class Enemy : public Entity {
public:
    enum Direction {HORIZONTAL, VERTICAL};
    Direction direction;
    bool movingRight;
    bool movingUp;

    Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, Direction dir);
    void update(GLfloat dt, GLuint width, GLuint height);

private:

};

#endif