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

glm::vec2 Enemy::getOverlaps(Entity& other) {
    glm::vec2 overlaps;
    overlaps.x = getOverlapX(other);
    overlaps.y = getOverlapY(other);

    return overlaps;
}

Enemy::Direction Enemy::getCollisionSide(Entity& other, Direction xDir, Direction yDir) {
    glm::vec2 overlaps = getOverlaps(other);
    Direction collisionSide = std::min(overlaps.x, overlaps.y) == overlaps.x ? xDir : yDir;
    return collisionSide;
}

std::tuple<bool, Enemy::Direction> Enemy::checkEnemyCollision(Enemy& other) {

    bool collided = checkCollision(other);

    if(!collided) {
        // No need to check the collision side
        Direction unused = UP;
        return std::make_tuple(false, unused);
    }

    // This is the side of the current Enemy instance that experiences a collision with 'other'
    Direction collisionSide;
    switch(direction) {
    case UP:
        switch(other.direction) {
        case DOWN:
            collisionSide = UP;
            break;
        case RIGHT:
            collisionSide = getCollisionSide(other, LEFT, UP);
            break;
        case LEFT:
            collisionSide = getCollisionSide(other, RIGHT, UP);
            break;
        default:
            break;
        }
        break;
    case DOWN:
        switch(other.direction) {
        case UP:
            collisionSide = DOWN;
            break;
        case RIGHT:
            collisionSide = getCollisionSide(other, LEFT, DOWN);
            break;
        case LEFT:
            collisionSide = getCollisionSide(other, RIGHT, DOWN);
            break;
        default:
            break;
        }
        break;
    case LEFT:
        switch(other.direction) {
        case RIGHT:
            collisionSide = LEFT;
            break;
        case UP:
            collisionSide = getCollisionSide(other, LEFT, DOWN);
            break;
        case DOWN:
            collisionSide = getCollisionSide(other, LEFT, UP);
            break;
        default:
            break;
        }
        break;
    case RIGHT:
        switch(other.direction) {
        case LEFT:
            collisionSide = RIGHT;
            break;
        case UP:
            collisionSide = getCollisionSide(other, RIGHT, DOWN);
            break;
        case DOWN:
            collisionSide = getCollisionSide(other, RIGHT, UP);
            break;
        default:
            break;
        }
        break;
    }

    return std::make_tuple(collided, collisionSide);
}

void Enemy::onCollision(Entity& other) {
    reverseDirection();
}