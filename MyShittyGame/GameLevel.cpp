#include "GameLevel.h"

#include <fstream>
#include <sstream>

void GameLevel::load(const GLchar *file, GLuint levelWidth, GLuint levelHeight)
{
    // TODO: Figure out scheme to store level data.

    // Clear old data
    this->arena.clear();
    // Load from file
    GLuint tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<GLuint>> tileData;
    if(fstream)
    {
        while(std::getline(fstream, line)) // Read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            while(sstream >> tileCode) // Read each word seperated by spaces
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if(tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::draw(EntityRenderer &renderer)
{
    for(Entity& wall : arena) {
        wall.draw(renderer);
    }
    for(Enemy& enemy : enemies) {
        enemy.draw(renderer);
    }
}

//GLboolean GameLevel::isCompleted()
//{
//    for(Entity &tile : this->bricks)
//        if(!tile.isSolid && !tile.destroyed)
//            return GL_FALSE;
//    return GL_TRUE;
//}

void GameLevel::init() {
    // TODO: Get this data from a file (JSON?)
    Entity top(glm::vec2(150, 0), glm::vec2(900, 20));
    Entity bottom(glm::vec2(150, 780), glm::vec2(900, 20));
    Entity right(glm::vec2(1030, 20), glm::vec2(20, 860));
    Entity leftTop(glm::vec2(150, 20), glm::vec2(20, 253));
    Entity leftBottom(glm::vec2(150, 526), glm::vec2(20, 254));
    
    top.isSolid = true;
    bottom.isSolid = true;
    right.isSolid = true;
    leftTop.isSolid = true;
    leftBottom.isSolid = true;

    arena.push_back(top);
    arena.push_back(bottom);
    arena.push_back(right);
    arena.push_back(leftTop);
    arena.push_back(leftBottom);
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();
    GLfloat unit_width = levelWidth / static_cast<GLfloat>(width), unit_height = levelHeight / height;
    for(GLuint y = 0; y < height; ++y)
    {
        for(GLuint x = 0; x < width; ++x)
        {
            // Check block type from level data (2D level array)
            if(tileData[y][x] == 1) // Solid
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                Entity obj(pos, size, ResourceManager::getTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.isSolid = GL_TRUE;
                this->arena.push_back(obj);
            }
            else if(tileData[y][x] > 1)	// Non-solid; now determine its color based on level data
            {
                glm::vec3 color = glm::vec3(1.0f); // original: white
                if(tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if(tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if(tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if(tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->arena.push_back(Entity(pos, size, ResourceManager::getTexture("block"), color));
            }
        }
    }
}