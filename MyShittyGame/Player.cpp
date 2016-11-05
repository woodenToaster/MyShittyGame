#include "Player.h"

Player::Player(glm::vec2 playerPos, glm::vec2 playerSize) : lives(3), alive(true), velocity(400.0f), startingPos(playerPos),
Entity(playerPos, playerSize, glm::vec3(0.0f, 1.0f, 0.0f), PLAYER) {

}

void Player::moveUp(GLfloat dt, std::vector<Entity> walls) {
    if(position.y >= 0) {
        position.y -= dt * velocity;
        for(Entity& wall : walls) {
            if(checkCollision(wall)) {
                position.y = wall.position.y + wall.size.y;
            }
        }
        if(position.y < 0) {
            position.y = 0;
        }
    }
}

void Player::moveDown(GLfloat dt, GLuint screenHeight, std::vector<Entity> walls) {
    if(position.y <= screenHeight - size.y) {
        position.y += dt * velocity;
        for(Entity& wall : walls) {
            if(checkCollision(wall)) {
                position.y = wall.position.y - size.y;
            }
        }
        if(position.y + size.y > screenHeight) {
            position.y = screenHeight - size.y;
        }
    }
}

void Player::moveLeft(GLfloat dt, std::vector<Entity> walls) {
    if(position.x >= 0) {
        position.x -= dt * velocity;
        for(Entity& wall : walls) {
            if(checkCollision(wall)) {
                position.x = wall.position.x + wall.size.x;
            }
        }
        if(position.x < 0) {
            position.x = 0;
        }
    }
}

void Player::moveRight(GLfloat dt, GLuint screenWidth, std::vector<Entity> walls) {
    if(position.x <= screenWidth - size.x) {
        position.x += dt * velocity;
        for(Entity& wall : walls) {
            if(checkCollision(wall)) {
                position.x = wall.position.x - size.x;
            }
        }
        if(position.x + size.x > screenWidth) {
            position.x = screenWidth - size.x;
        }
    }
}

void Player::onCollision(Entity& other) {
    switch(other.type) {
        case WALL:
            break;
        case ENEMY:
            lives--;
            position = startingPos;
            break;
    }
}