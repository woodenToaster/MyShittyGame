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
: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f), sprite(sprite), isSolid(false), destroyed(false) { }

Entity::Entity(glm::vec2 pos, glm::vec2 size, glm::vec3 color, glm::vec2 velocity)
: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f), sprite(), isSolid(false), destroyed(false) { }

void Entity::drawFromTexture(EntityRenderer &renderer)
{
    renderer.drawSpriteFromTexture(sprite, position, size, rotation, color);
}

void Entity::draw(EntityRenderer &renderer)
{
    renderer.drawSprite(position, size, rotation, color);
}

void Entity::update() {

}