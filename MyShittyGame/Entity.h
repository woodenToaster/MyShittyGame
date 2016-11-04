#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "EntityRenderer.h"

class Entity {

public:
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    glm::vec3 color;
    GLfloat rotation;
    GLboolean isSolid;
    GLboolean destroyed;
    Texture2d sprite;
    Entity();
    Entity(glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    Entity(glm::vec2 pos, glm::vec2 size, Texture2d sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    virtual void draw(EntityRenderer &renderer);
    virtual void drawFromTexture(EntityRenderer &renderer);
    virtual void update();
};

#endif