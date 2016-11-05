#include "Game.h"
#include "ResourceManager.h"
#include "EntityRenderer.h"
#include "Player.h"

#define ENTITY_SIZE glm::vec2(40, 40)

EntityRenderer *renderer;
Player *player;

Game::Game(GLuint width, GLuint height) :
    state(GAME_ACTIVE),
    keys(),
    width(width),
    height(height) {

}

Game::~Game() {
    delete renderer;
    delete player;
}

void Game::init() {
    ResourceManager::loadShader("SimpleVertexShader.vert", "SimpleFragmentShader.frag", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
    // ResourceManager::getShader("sprite").use().setInteger("image", 0);
    ResourceManager::getShader("sprite").use().setMatrix4("projection", projection);

    // ResourceManager::loadTexture("textures/awesomeface.png", GL_TRUE, "face");
    renderer = new EntityRenderer(ResourceManager::getShader("sprite"));

    glm::vec2 playerPos = glm::vec2(0.0f, 400.0f);
    player = new Player(playerPos, ENTITY_SIZE);

    GameLevel levelOne;
    levelOne.init();

    Enemy horizontalEnemy(glm::vec2(400, 600), ENTITY_SIZE, glm::vec3(0.0f, 0.0f, 1.0f), Enemy::HORIZONTAL);
    Enemy verticalEnemy(glm::vec2(800, 400), ENTITY_SIZE, glm::vec3(1.0f, 0.0f, 0.0f), Enemy::VERTICAL);

    levelOne.enemies.push_back(horizontalEnemy);
    levelOne.enemies.push_back(verticalEnemy);
    levels.push_back(levelOne);
    level = 0;
}

void Game::updateEnemies(GLfloat dt) {
    for(auto& enemy : levels[level].enemies) {
        enemy.update(dt, width, height);
    }
}

void Game::checkCollisions() {

    // Check player collisions with enemies
    for(Enemy& enemy : levels[level].enemies) {
        if(player->checkCollision(enemy)) {
            player->onCollision(enemy);
        }
    }

    // Check enemy collisions
    for(Enemy& enemy : levels[level].enemies) {

        // With other enemies
        for(Enemy& otherEnemy : levels[level].enemies) {
            if(&otherEnemy == &enemy) {
                continue;
            }
            if(enemy.checkCollision(otherEnemy)) {
                enemy.onCollision(otherEnemy);
            }
        }

        // With arena walls
        for(Entity& wall : levels[level].arena) {
            if(enemy.checkCollision(wall)) {
                enemy.onCollision(wall);
            }
        }
    }
}

void Game::update(GLfloat dt) {
    updateEnemies(dt);
    checkCollisions();
}


void Game::processInput(GLfloat dt) {
    if(state == GAME_ACTIVE)
    {
        if(keys[GLFW_KEY_LEFT]) {
            player->moveLeft(dt, levels[level].arena);
        }
        if(keys[GLFW_KEY_RIGHT]) {
            player->moveRight(dt, width, levels[level].arena);
        }
        if(keys[GLFW_KEY_UP]) {
            player->moveUp(dt, levels[level].arena);
        }
        if(keys[GLFW_KEY_DOWN]) {
            player->moveDown(dt, height, levels[level].arena);
        }
    }
}

void Game::render() {
    if(state == GAME_ACTIVE) {
        player->draw(*renderer);
        levels[level].draw(*renderer);
    }
}