#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <irrKlang.h>

#include "Entity.h"
#include "Enemy.h"
#include "EntityRenderer.h"
#include "ResourceManager.h"

class GameLevel
{
public:
    std::vector<Entity> arena;
    std::vector<Enemy> enemies;
    glm::vec2 doorPosition;
    glm::vec2 doorSize;
    glm::vec3 doorColor;
    glm::vec2 exitPosition;
    glm::vec2 exitSize;
    glm::vec3 exitColor;
    bool isCompleted;

    GameLevel();
    void load(const GLchar *file);
    void loadWalls(std::istream& fstream);
    void loadEnemies(std::istream& fstream);
    void draw(EntityRenderer &renderer);
    void init(std::string name);
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
private:
    
};

#endif