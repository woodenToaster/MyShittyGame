#include "Enemy.h"

Enemy::Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, glm::vec3 color, Direction dir) : direction(dir), velocity(500.0f),
    Entity(enemyPosition, enemySize, color, ENEMY) {
}

void Enemy::update(float dt, GLuint width, GLuint height) {
    GLfloat velocityDelta = velocity * dt;
    switch(direction) {
        case HORIZONTAL:
            if(position.x >= 0 && !movingRight)
                position.x -= velocityDelta;
            if(position.x <= width - size.x)
                position.x += velocityDelta;

            if(position.x <= 0 || position.x + size.x >= width) {
                velocity = -velocity;
                movingRight = !movingRight;
            }
            break;
        case VERTICAL:
            if(position.y >= 0 && !movingUp)
                position.y -= velocityDelta;
            if(position.y <= height - size.y)
                position.y += velocityDelta;

            if(position.y <= 0 || position.y + size.y >= height) {
                velocity = -velocity;
                movingUp = !movingUp;;
            }
            break;
    }
}

void Enemy::onCollision(Entity& other) {
    velocity = -velocity;

}