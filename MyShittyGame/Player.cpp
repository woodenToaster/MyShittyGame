#include "Player.h"

Player::Player(glm::vec2 playerPos, glm::vec2 playerSize, irrklang::ISoundEngine* soundEngine) :
    lives(3),
    alive(true),
    velocity(400.0f),
    startingPos(playerPos),
    soundEngine(soundEngine),
    Entity(playerPos, playerSize, glm::vec3(0.0f, 1.0f, 0.0f), PLAYER) {

}

void Player::moveUp(GLfloat dt, std::vector<Entity> walls) {
    if(position.y >= 0) {
        position.y -= dt * velocity;
        for(Entity& wall : walls) {
            if(checkCollision(wall) && wall.isSolid) {
                position.y = wall.getBottomSide();
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
            if(checkCollision(wall) && wall.isSolid) {
                position.y = wall.getY() - size.y;
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
            if(checkCollision(wall) && wall.isSolid) {
                position.x = wall.getRightSide();
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
            if(checkCollision(wall) && wall.isSolid) {
                position.x = wall.getX() - size.x;
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
            soundEngine->play2D("sounds/bleep.ogg", false);
            break;
        // case EXIT:
    }
}