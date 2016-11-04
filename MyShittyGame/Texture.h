#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture2d
{
public:
    GLuint id;
    GLuint width;
    GLuint height;
    GLuint internalFormat;
    GLuint imageFormat;
    GLuint wrapS;
    GLuint wrapT;
    GLuint filterMin;
    GLuint filterMax;
    
    Texture2d();
    void generate(GLuint width, GLuint height, unsigned char* data);
    void bind() const;
};

#endif