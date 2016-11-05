#include "Player.h"

Player::Player(glm::vec2 playerPos, glm::vec2 playerSize) : lives(3), alive(true), velocity(400.0f),
Entity(playerPos, playerSize, glm::vec3(0.0f, 1.0f, 0.0f)) {

}

void Player::moveUp(GLfloat dt) {
    if(position.y >= 0) {
        position.y -= dt * velocity;
        if(position.y < 0) {
            position.y = 0;
        }
    }
}

void Player::moveDown(GLfloat dt, GLuint screenHeight) {
    if(position.y <= screenHeight - size.y) {
        position.y += dt * velocity;
        if(position.y + size.y > screenHeight) {
            position.y = screenHeight - size.y;
        }
    }
}

void Player::moveLeft(GLfloat dt) {
    if(position.x >= 0) {
        position.x -= dt * velocity;
        if(position.x < 0) {
            position.x = 0;
        }
    }
}

void Player::moveRight(GLfloat dt, GLuint screenWidth) {
    if(position.x <= screenWidth - size.x) {
        position.x += dt * velocity;
        if(position.x + size.x > screenWidth) {
            position.x = screenWidth - size.x;
        }
    }
}