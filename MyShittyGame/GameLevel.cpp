#include "GameLevel.h"

#include <iostream>
#include <sstream>

#define ENTITY_SIZE glm::vec2(40, 40)

void GameLevel::load(const GLchar *file)
{
    arena.clear();
    std::string line;
    std::ifstream fstream(file);
    if(fstream)
    {
        while(std::getline(fstream, line)) {
            loadWalls(fstream);
            loadEnemies(fstream);
        }
        // this->init(tileData, levelWidth, levelHeight);
        fstream.close();
    }
}

void GameLevel::loadWalls(std::istream& fstream) {
    std::string line;
    std::string state;
    GLfloat temp;

    while(std::getline(fstream, line)) {
        if(line.compare("enemies") == 0) {
            return;
        }
        if(line.compare("door") == 0) {
            state = "door";
            continue;
        }
        if(line.compare("exit") == 0) {
            state = "exit";
            continue;
        }
        std::vector<GLfloat> data;
        std::istringstream sstream(line);
        while(sstream >> temp) {
            data.push_back(temp);
            if(sstream.peek() == ',') {
                sstream.ignore();
            }
        }
        glm::vec2 wallPos = glm::vec2(data[0], data[1]);
        glm::vec2 wallSize = glm::vec2(data[2], data[3]);
        glm::vec3 wallColor = glm::vec3(data[4], data[5], data[6]);
        if(state.compare("door") == 0) {
            doorPosition = wallPos;
            doorSize = wallSize;
            doorColor = wallColor;
        }
        else if(state.compare("exit") == 0) {
            exitPosition = wallPos;
            exitSize = wallSize;
            exitColor = wallColor;
            Entity exit(wallPos, wallSize, wallColor);
            arena.insert(arena.begin(), exit);
        }
        else {
            Entity wall(wallPos, wallSize, wallColor);
            arena.push_back(wall);
        }
    }
}

void GameLevel::loadEnemies(std::istream& fstream) {
    std::string line;
    GLfloat temp;

    while(std::getline(fstream, line)) {
        if(line.empty())
            return;

        std::vector<GLfloat> data;
        std::istringstream sstream(line);
        while(sstream >> temp) {
            data.push_back(temp);
            if(sstream.peek() == ',') {
                sstream.ignore();
            }
        }
        glm::vec2 pos = glm::vec2(data[0], data[1]);
        glm::vec2 size = glm::vec2(data[2], data[3]);
        glm::vec3 color = glm::vec3(data[4], data[5], data[6]);
        Enemy::Direction dir = static_cast<Enemy::Direction>(static_cast<int>(data[7]));
        Enemy enemy(pos, size, color, dir);
        enemies.push_back(enemy);
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

void GameLevel::init() {
    load("levels\\level1.txt");
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