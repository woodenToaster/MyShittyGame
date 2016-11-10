#include <algorithm>
#include <iostream>
#include "Enemy.h"

Enemy::Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, glm::vec3 color, EnemyType enemyType) : enemyType(enemyType), velocity(750.0f),
    Entity(enemyPosition, enemySize, color, ENEMY) {

    if(enemyType == HORIZONTAL) {
        direction = RIGHT;
    }
    else if(enemyType == VERTICAL) {
        direction = DOWN;
    }
}

void Enemy::undoWallOverlap(std::vector<Entity>& walls) {
    for(Entity& wall : walls) {
        if(this->checkCollision(wall)) {
            switch(direction) {
            case LEFT:
                position.x = wall.position.x + wall.size.x;
                break;
            case RIGHT:
                position.x = wall.position.x - size.x;
                break;
            case UP:
                position.y = wall.position.y + wall.size.y;
                break;
            case DOWN:
                position.y = wall.position.y - size.y;
                break;
            }
            onCollision(wall);
            return;
        }
    }
}

void Enemy::reverseDirection() {
    if(enemyType == VERTICAL) {
        direction = direction == UP ? DOWN : UP;
    }
    else if(enemyType == HORIZONTAL) {
        direction = direction == RIGHT ? LEFT : RIGHT;
    }
    velocity = -velocity;
    rotation = -rotation;
}

void Enemy::undoEnemyOverlap(Enemy& other) {
    std::tuple<bool, Direction> collision = checkEnemyCollision(other);
    if(std::get<0>(collision)) {
        switch(std::get<1>(collision)) {
        case UP:
            switch(direction) {
            case UP:
                position.y = other.position.y + other.size.y;
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
                other.position.y = position.y + size.y;
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
                position.x = other.position.x + other.size.x;
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
    return std::abs(position.x - other.position.x);
}

GLfloat Enemy::getOverlapY(Entity& other) {
    return std::abs(position.y - other.position.y);
}


std::tuple<bool, Enemy::Direction> Enemy::checkEnemyCollision(Enemy& other) {

    bool collisionX = position.x + size.x > other.position.x &&
        other.position.x + other.size.x > position.x;

    bool collisionY = position.y + size.y > other.position.y &&
        other.position.y + other.size.y > position.y;

    bool collided = collisionX && collisionY;
    
    // This is the side of the current instance that experiences a collision with 'other'
    Direction collisionSide;
    if(direction == UP) {
        if(other.direction == DOWN) {
            collisionSide = UP;
        }
        else if(other.direction == RIGHT) {
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
        else if(other.direction == RIGHT) {
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
        if(other.direction == RIGHT) {
            collisionSide = LEFT;
        }
        else if(other.direction == UP) {
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
    else if(direction == RIGHT) {
        if(other.direction == LEFT) {
            collisionSide = RIGHT;
        }
        else if(other.direction == UP) {
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

    /*switch(other.type) {
        case WALL:
            velocity = -velocity;
            rotation = -rotation;
            reverseDirection();
            break;
    }*/
}