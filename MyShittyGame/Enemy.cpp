#include <algorithm>
#include <iostream>
#include "Enemy.h"

Enemy::Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, glm::vec3 color, EnemyType enemyType) : 
    enemyType(enemyType),
    velocity(750.0f),
    Entity(enemyPosition, enemySize, color, ENEMY) {

    direction = getStartingDirection();
}

Enemy::Direction Enemy::getStartingDirection() {
    return isHorizontal() ? RIGHT : DOWN;
}

bool Enemy::isHorizontal() {
    return enemyType == HORIZONTAL;
}

bool Enemy::isVertical() {
    return enemyType == VERTICAL;
}

Enemy::EnemyType Enemy::getType() {
    return enemyType;
}

void Enemy::setType(EnemyType newType) {
    enemyType = newType;
}

void Enemy::swapType() {
    enemyType = isHorizontal() ? VERTICAL : HORIZONTAL;
}

void Enemy::undoWallOverlap(std::vector<Entity>& walls) {
    for(Entity& wall : walls) {
        if(this->checkCollision(wall)) {
            switch(direction) {
            case LEFT:
                position.x = wall.getRightSide();
                break;
            case RIGHT:
                position.x = wall.getX() - size.x;
                break;
            case UP:
                position.y = wall.getBottomSide();
                break;
            case DOWN:
                position.y = wall.getY() - size.y;
                break;
            }
            onCollision(wall);
            return;
        }
    }
}

void Enemy::reverseDirection() {
    if(isVertical()) {
        direction = isTravelingUp() ? DOWN : UP;
    }
    else if(isHorizontal()) {
        direction = isTravelingRight() ? LEFT : RIGHT;
    }
    velocity = -velocity;
    rotation = -rotation;
}

bool Enemy::isTravelingRight() {
    return direction == RIGHT;
}

bool Enemy::isTravelingUp() {
    return direction == UP;
}

void Enemy::undoEnemyOverlap(Enemy& other) {
    std::tuple<bool, Direction> collision = checkEnemyCollision(other);
    if(std::get<0>(collision)) {
        switch(std::get<1>(collision)) {
        case UP:
            switch(direction) {
            case UP:
                position.y = other.getBottomSide();
                break;
            case LEFT:
            case RIGHT:
                other.position.y = position.y - other.size.y;
                break;
            }
            break;
        case DOWN:
            switch(direction) {
            case DOWN:
                position.y = other.position.y - size.y;
                break;
            case LEFT:
            case RIGHT:
                other.position.y = getBottomSide();
                break;
            }
            break;
        case LEFT:
            switch(direction) {
            case UP:
            case DOWN:
                other.position.x = position.x - other.size.x;
                break;
            case LEFT:
                position.x = other.getRightSide();
                break;
            }
            break;
        case RIGHT:
            switch(direction) {
            case UP:
            case DOWN:
                other.position.x = position.x + size.x;
                break;
            case RIGHT:
                position.x = other.position.x - size.x;
                break;
            }
            break;
        }
    }
}

void Enemy::update(float dt, GLuint width, GLuint height, std::vector<Entity> walls) {
    GLfloat velocityDelta = velocity * dt;
    // rotation += 5.0f;
    switch(enemyType) {
        case HORIZONTAL:
            position.x += velocityDelta;
            undoWallOverlap(walls);

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
            position.y += velocityDelta;
            undoWallOverlap(walls);

            if(position.y <= 0) {
                position.y = 0;
                reverseDirection();
            }
            else if(position.y + size.y >= height) {
                position.y = height - size.y;
                reverseDirection();
            }
            break;
    }
}

GLfloat Enemy::getOverlapX(Entity& other) {
    return std::abs(position.x - other.getX());
}

GLfloat Enemy::getOverlapY(Entity& other) {
    return std::abs(position.y - other.getY());
}


std::tuple<bool, Enemy::Direction> Enemy::checkEnemyCollision(Enemy& other) {

    bool collided = checkCollision(other);

    // This is the side of the current instance that experiences a collision with 'other'
    Direction collisionSide;
    if(isTravelingUp()) {
        if(other.direction == DOWN) {
            collisionSide = UP;
        }
        else if(other.isTravelingRight()) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? LEFT : UP;
        }
        else if(other.direction == LEFT) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? RIGHT : UP;
        }
    }
    else if(direction == DOWN) {
        if(other.direction == UP) {
            collisionSide = DOWN;
        }
        else if(other.isTravelingRight()) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? LEFT : DOWN;
        }
        else if(other.direction == LEFT) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? RIGHT : DOWN;
        }
    }
    else if(direction == LEFT) {
        if(other.isTravelingRight()) {
            collisionSide = LEFT;
        }
        else if(other.isTravelingUp()) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? LEFT : DOWN;
        }
        else if(other.direction == DOWN) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? LEFT : UP;
        }
    }
    else if(isTravelingRight()) {
        if(other.direction == LEFT) {
            collisionSide = RIGHT;
        }
        else if(other.isTravelingUp()) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? RIGHT : DOWN;
        }
        else if(other.direction == DOWN) {
            GLfloat xoverlap = getOverlapX(other);
            GLfloat yoverlap = getOverlapY(other);
            collisionSide = std::min(xoverlap, yoverlap) == xoverlap ? RIGHT : UP;
        }
    }
    return std::make_tuple(collided, collisionSide);
}

void Enemy::onCollision(Entity& other) {
    reverseDirection();
}