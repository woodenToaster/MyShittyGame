#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Entity.h"
#include "Enemy.h"
#include "EntityRenderer.h"
#include "ResourceManager.h"

class GameLevel
{
public:
    std::vector<Entity> bricks;
    std::vector<Enemy> enemies;

    GameLevel() {}

    void load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    void draw(EntityRenderer &renderer);
    GLboolean isCompleted();

private:
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif