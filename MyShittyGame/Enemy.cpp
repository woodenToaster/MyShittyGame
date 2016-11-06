#include "Enemy.h"

Enemy::Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, glm::vec3 color, Direction dir) : direction(dir), velocity(750.0f),
    Entity(enemyPosition, enemySize, color, ENEMY) {

    movingRight = direction == HORIZONTAL ? true : false;
    movingUp = direction == VERTICAL ? true : false;
}

void Enemy::undoOverlap(std::vector<Entity> walls) {
    for(Entity& wall : walls) {
        if(checkCollision(wall)) {
            if(!movingRight) {
                position.x = wall.position.x + wall.size.x;
            }
            if(movingRight) {
                position.x = wall.position.x - size.x;
            }
            onCollision(wall);
        }
    }
}

void Enemy::update(float dt, GLuint width, GLuint height, std::vector<Entity> walls) {
    GLfloat velocityDelta = velocity * dt;
    switch(direction) {
        case HORIZONTAL:
            if(position.x >= 0 && !movingRight) {
                position.x += velocityDelta;
                //undoOverlap(walls);
            }
            if(position.x <= width - size.x && movingRight) {
                position.x += velocityDelta;
                //undoOverlap(walls);
            }
            if(position.x <= 0) {
                position.x = 0;
                onCollision(Entity());
            }
            else if(position.x + size.x >= width) {
                position.x = width - size.x;
                onCollision(Entity());
            }
            break;
        case VERTICAL:
            if(position.y >= 0 && movingUp) {
                position.y += velocityDelta;
                // undoOverlap(walls);
            }
            if(position.y <= height - size.y && !movingUp) {
                position.y += velocityDelta;
                // undoOverlap(walls);
            }
            if(position.y <= 0) {
                position.y = 0;
                onCollision(Entity());
            }
            else if(position.y + size.y >= height) {
                position.y = height - size.y;
                onCollision(Entity());
            }
            break;
    }
}

void Enemy::onCollision(Entity& other) {
    velocity = -velocity;
    rotation = -rotation;
    if(direction == HORIZONTAL) {
        movingRight = !movingRight;
    }
    if(direction == VERTICAL) {
        movingUp = !movingUp;
    }
    /*switch(other.type) {
        case WALL:
            velocity = -velocity;
            rotation = -rotation;
            if(direction == HORIZONTAL) {
                movingRight = !movingRight;
            }
            if(direction == VERTICAL) {
                movingUp = !movingUp;
            }
            break;
    }*/
}