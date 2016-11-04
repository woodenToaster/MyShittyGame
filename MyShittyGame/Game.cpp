#include "Game.h"
#include "ResourceManager.h"
#include "EntityRenderer.h"
#include "Entity.h"

EntityRenderer *renderer;
Entity *player;

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
    player = new Entity(playerPos, PLAYER_SIZE);

    GameLevel levelOne;
    Enemy horizontalEnemy(glm::vec2(400, 600), ENEMY_SIZE, Enemy::HORIZONTAL);
    horizontalEnemy.color = glm::vec3(0.0f, 0.0f, 1.0f);
    Enemy verticalEnemy(glm::vec2(800, 400), ENEMY_SIZE, Enemy::VERTICAL);
    verticalEnemy.color = glm::vec3(1.0f, 0.0f, 0.0f);
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

void Game::update(GLfloat dt) {
    updateEnemies(dt);
}


void Game::processInput(GLfloat dt) {
    if(state == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        if(keys[GLFW_KEY_LEFT]) {
            if(player->position.x >= 0)
                player->position.x -= velocity;
        }
        if(keys[GLFW_KEY_RIGHT]) {
            if(player->position.x <= width - player->size.x)
                player->position.x += velocity;
        }
        if(keys[GLFW_KEY_UP]) {
            if(player->position.y >= 0) {
                player->position.y -= velocity;
            }
        }
        if(keys[GLFW_KEY_DOWN]) {
            if(player->position.y <= height - player->size.y) {
                player->position.y += velocity;
            }
        }
    }
}

void Game::render() {
    
    if(state == GAME_ACTIVE) {
        

        player->draw(*renderer);

        // TODO: Refactor arena code into its own class
        renderer->drawSprite(glm::vec2(150, 0), glm::vec2(900, 20), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->drawSprite(glm::vec2(150, 780), glm::vec2(900, 20), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->drawSprite(glm::vec2(1030, 20), glm::vec2(20, 860), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->drawSprite(glm::vec2(150, 20), glm::vec2(20, 253), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderer->drawSprite(glm::vec2(150, 526), glm::vec2(20, 254), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        levels[level].draw(*renderer);
    }
}