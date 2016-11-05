#include "Enemy.h"

Enemy::Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, Direction dir) : direction(dir), velocity(400.0f),
    Entity(enemyPosition, enemySize) {
}

void Enemy::update(float dt, GLuint width, GLuint height) {
    GLfloat velocityDelta = velocity * dt;
    switch(direction) {
        case HORIZONTAL:
            if(position.x >= 0 && !movingRight)
                position.x -= velocityDelta;
            if(position.x <= width - size.x && movingRight)
                position.x += velocityDelta;

            if(position.x <= 0 || position.x + size.x >= width) {
                movingRight = !movingRight;
            }
            break;
        case VERTICAL:
            if(position.y >= 0 && movingUp)
                position.y -= velocityDelta;
            if(position.y <= height - size.y && !movingUp)
                position.y += velocityDelta;

            if(position.y <= 0 || position.y + size.y >= height) {
                movingUp = !movingUp;
            }
            break;
    }
}