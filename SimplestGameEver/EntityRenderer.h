#ifndef ENTITY_RENDERER_H
#define ENTITY_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

class EntityRenderer
{
public:
    EntityRenderer(Shader &shader);
    ~EntityRenderer();
    void drawSpriteFromTexture(Texture2d &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void drawSprite(glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
    Shader shader;
    GLuint quadVAO;

    void initRenderData();
};

#endif