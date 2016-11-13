#include "Entity.h"

Entity::Entity(): 
    position(0, 0), 
    size(1, 1), 
    velocity(0.0f),
    color(1.0f),
    rotation(0.0f),
    sprite(),
    isSolid(false),
    destroyed(false) { }

Entity::Entity(glm::vec2 pos, glm::vec2 size, Texture2d sprite, glm::vec3 color, glm::vec2 velocity)
: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f), sprite(sprite), isSolid(false), destroyed(false) {}

Entity::Entity(glm::vec2 pos, glm::vec2 size, glm::vec3 color, EntityType type)
: position(pos), size(size), color(color), rotation(0.0f), sprite(), isSolid(false), destroyed(false), type(type) { }

GLfloat Entity::getBottomSide() {
    return position.y + size.y;
}

GLfloat Entity::getRightSide() {
    return position.x + size.x;
}

GLfloat Entity::getX() {
    return position.x;
}

GLfloat Entity::getY() {
    return position.y;
}

GLfloat Entity::getSizeX() {
    return size.x;
}

GLfloat Entity::getSizeY() {
    return size.y;
}

void Entity::setX(GLfloat x) {
    position.x = x;
}

void Entity::setY(GLfloat y) {
    position.y = y;
}

void Entity::setPosition(glm::vec2 pos) {
    position = pos;
}

void Entity::drawFromTexture(EntityRenderer &renderer) {
    renderer.drawSpriteFromTexture(sprite, position, size, rotation, color);
}

void Entity::draw(EntityRenderer &renderer) {
    renderer.drawSprite(position, size, rotation, color);
}

void Entity::update() {

}

bool Entity::checkCollision(Entity& other) {
    // Collision x-axis?
    bool collisionX = position.x + size.x > other.position.x &&
        other.position.x + other.size.x > position.x;
    // Collision y-axis?
    bool collisionY = position.y + size.y > other.position.y &&
        other.position.y + other.size.y > position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

void Entity::onCollision(Entity& other) {

}