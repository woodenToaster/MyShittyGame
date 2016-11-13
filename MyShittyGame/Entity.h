#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "EntityRenderer.h"

class Entity {

public:
    enum EntityType { PLAYER, ENEMY, WALL };
    EntityType type;
    glm::vec2 velocity;
    glm::vec3 color;
    GLfloat rotation;
    GLboolean isSolid;
    GLboolean destroyed;
    Texture2d sprite;

    Entity();
    Entity(glm::vec2 pos, glm::vec2 size, glm::vec3 color, EntityType type = WALL);
    Entity(glm::vec2 pos, glm::vec2 size, Texture2d sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    GLfloat getBottomSide();
    GLfloat getRightSide();

    GLfloat getX();
    GLfloat getY();

    GLfloat getSizeX();
    GLfloat getSizeY();

    void setX(GLfloat x);
    void setY(GLfloat y);
    void setPosition(glm::vec2 pos);

    virtual void draw(EntityRenderer &renderer);
    virtual void drawFromTexture(EntityRenderer &renderer);
    virtual void update();
    virtual bool checkCollision(Entity& other);
    virtual void onCollision(Entity& other);

protected:
    glm::vec2 position;
    glm::vec2 size;
};

#endif