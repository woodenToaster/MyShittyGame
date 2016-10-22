#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 translationMatrix;

void main() {

    gl_Position = translationMatrix * vec4(vertexPosition_modelspace, 1.0);
}