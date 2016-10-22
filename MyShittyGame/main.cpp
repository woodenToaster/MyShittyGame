#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
void update(GLFWwindow* window);
void updateEnemy();

static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

float xOffset = 0;
float yOffset = 0;
float xOffsetEnemy = 0;
float yOffsetEnemy = 0;
bool enemyGoingUp = true;

static GLfloat playerVertexData[] = {
    -0.1f, 0.1f, 0.0f,
    0.0f, 0.1f, 0.0f,
    -0.1f, 0.0f, 0.0f,
    0.0f, 0.1f, 0.0f,
    0.0f, 0.0f, 0.0f,
    -0.1f, 0.0f, 0.0f,
};

static GLfloat enemyVertexData[] = {
    -0.1f, 0.1f, 0.0f,
    0.0f, 0.1f, 0.0f,
    -0.1f, 0.0f, 0.0f,
    0.0f, 0.1f, 0.0f,
    0.0f, 0.0f, 0.0f,
    -0.1f, 0.0f, 0.0f,
};

int main(int argc, char* argv[]) {
    
    static GLFWwindow *win;
    int width = 0, height = 0;

    glfwSetErrorCallback(error_callback);
    if(!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    glewExperimental = 1;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    GLuint playerVertexArrayId;
    glGenVertexArrays(1, &playerVertexArrayId);
    glBindVertexArray(playerVertexArrayId);

    GLuint enemyVertexArrayId;
    glGenVertexArrays(1, &enemyVertexArrayId);
    glBindVertexArray(enemyVertexArrayId);

    GLuint playerVertexBuffer;
    glGenBuffers(1, &playerVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, playerVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertexData), playerVertexData, GL_DYNAMIC_DRAW);


    GLuint enemyVertexBuffer;
    glGenBuffers(1, &enemyVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(enemyVertexData), enemyVertexData, GL_DYNAMIC_DRAW);


    glfwSetKeyCallback(win, keyCallback);
    GLuint programId = loadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
    GLint translationMatrixLocation = glGetUniformLocation(programId, "translationMatrix");
    GLint inputColorLocation = glGetUniformLocation(programId, "inputColor");

    while(!glfwWindowShouldClose(win)) {

        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);

        // Player uniforms
        glm::mat4 transMatrix = glm::translate(xOffset, yOffset, 0.0f);
        glm::vec3 inputColor = glm::vec3(0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);

        // Draw player
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, playerVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        // Enemy uniforms
        transMatrix = glm::translate(0.0f, yOffsetEnemy, 0.0f);
        inputColor = glm::vec3(1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);

        // Draw enemy
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, enemyVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        glfwPollEvents();
        update(win);
        updateEnemy();
        glfwSwapBuffers(win);
    }

    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch(key) {
        case GLFW_KEY_ESCAPE:
            if(action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        default:
            break;
    }
}

void update(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        yOffset += 0.01f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        yOffset -= 0.01f;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        xOffset -= 0.01f;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        xOffset += 0.01f;
    }
}

void updateEnemy() {

    if(yOffsetEnemy >= 1.0f && enemyGoingUp) {
        enemyGoingUp = false;
    }
    if(yOffsetEnemy <= -1.0f && !enemyGoingUp) {
        enemyGoingUp = true;
    }
    if(enemyGoingUp) {
        yOffsetEnemy += 0.02f;
    }
    else {
        yOffsetEnemy -= 0.02f;
    }
}

GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path){

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0){
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

