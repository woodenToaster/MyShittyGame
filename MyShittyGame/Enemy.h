#ifndef ENEMY_H
#define ENEMY_H

#include <tuple>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Entity.h"

class Enemy : public Entity {
public:
    enum Direction { UP, DOWN, LEFT, RIGHT };
    enum EnemyType { HORIZONTAL, VERTICAL };

    Enemy(glm::vec2 enemyPosition, glm::vec2 enemySize, glm::vec3 color, EnemyType enemyType);
    void undoWallOverlap(std::vector<Entity>& walls);
    void reverseDirection();
    void undoEnemyOverlap(Enemy& other);
    void update(GLfloat dt, GLuint width, GLuint height, std::vector<Entity> walls);
    GLfloat getOverlapX(Entity& other);
    GLfloat getOverlapY(Entity& other);
    std::tuple<bool, Direction> checkEnemyCollision(Enemy& other);
    void onCollision(Entity& other);
    EnemyType getType();
    void setType(EnemyType newType);
    void swapType();
    Direction getStartingDirection();
    bool isHorizontal();
    bool isVertical();
    bool isTravelingRight();
    bool isTravelingUp();
    glm::vec2 getOverlaps(Entity& other);
    Direction getCollisionSide(Entity& other, Direction xDir, Direction yDir);

private:
    Direction direction;
    EnemyType enemyType;
    GLfloat velocity;
};

#endif
