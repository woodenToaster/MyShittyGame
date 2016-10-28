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

struct Window {
    int width;
    int height;
    const int windowWidth = WINDOW_WIDTH;
    const int windowHeight = WINDOW_HEIGHT;
    GLFWwindow* win;

    ~Window() {
        glfwTerminate();
    }

    static void error_callback(int e, const char *d) {
        printf("Error %d: %s\n", e, d);
    }

    void init() {
        glfwSetErrorCallback(error_callback);

        if(!glfwInit()) {
            fprintf(stdout, "[GFLW] failed to init!\n");
            exit(1);
        }

        win = glfwCreateWindow(windowWidth, windowHeight, "My Shitty Game", NULL, NULL);
        glfwMakeContextCurrent(win);

        glewExperimental = 1;
        if(glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to setup GLEW\n");
            exit(1);
        }
    }
};

struct Entity {
    GLfloat *vertexData;
    GLuint size = 18;
    GLuint vertexArrayId;
    GLuint vertexBuffer;
    float xOffset = 0;
    float yOffset = 0;
    glm::vec3 startingPosition;
    glm::vec3 color;
    glm::mat4 transMatrix;
    glm::vec3 inputColor;

    enum direction {UP, DOWN, LEFT, RIGHT};

    Entity(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        color(entityColor),
        xOffset(startingPos.x),
        yOffset(startingPos.y),
        startingPosition(startingPos),
        vertexData(data),
        size(dataLength) {

        initVertexArray();
        initBuffer();
    }

    ~Entity() {
        delete[] vertexData;
        vertexData = nullptr;
    }

    void initVertexArray() {
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);
    }

    void initBuffer() {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* size, vertexData, GL_DYNAMIC_DRAW);
    }

    void draw() {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);
    }
};



struct DynamicEntity : Entity {

    enum direction { UP, DOWN, LEFT, RIGHT };

    float xSize = 0.1f;
    float ySize = 0.1f;

    DynamicEntity(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        Entity(entityColor, startingPos,data, dataLength) {}

    void updateUniforms(GLint translationMatrixLocation, GLint inputColorLocation) {
        transMatrix = glm::translate(xOffset, yOffset, 0.0f);
        inputColor = color;
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);
    }

    bool checkCollisions(DynamicEntity* other) {
        // Collision x-axis?
        bool collisionX = xOffset + xSize >= other->xOffset &&
            other->xOffset + other->xSize >= xOffset;
        // Collision y-axis?
        bool collisionY = yOffset + ySize >= other->yOffset &&
            other->yOffset + other->ySize >= yOffset;

        // Collision only if on both axes
        return collisionX && collisionY;
    }

    virtual void updatePosition(GLFWwindow* window) = 0;
    virtual void onCollision(DynamicEntity* other) = 0;
};

struct Player : DynamicEntity {
    bool dead = false;
    int lives = 3;
    float speed = 0.01f;

    Player(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength) {}

    void updatePosition(GLFWwindow* window) {
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            yOffset += speed;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            yOffset -= speed;
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            xOffset -= speed;
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            xOffset += speed;
        }
    }

    void onCollision(DynamicEntity* other) {
        dead = true;
        lives--;
        xOffset = startingPosition.x;
        yOffset = startingPosition.y;
    }
};

struct VerticalEnemy : DynamicEntity {
    bool movingUp = true;
    float speed = 0.02f;

    VerticalEnemy(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength) {}

    void updatePosition(GLFWwindow* window) {
        if(yOffset >= 1.0f && movingUp) {
            movingUp = false;
        }
        if(yOffset <= -1.0f && !movingUp) {
            movingUp = true;
        }
        if(movingUp) {
            yOffset += speed;
        }
        else {
            yOffset -= speed;
        }
    }

    void onCollision(DynamicEntity* other) {
        movingUp = !movingUp;
    }
};

struct HorizontalEnemy : DynamicEntity {
    bool movingRight = true;
    float speed = 0.02f;

    HorizontalEnemy(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength) {}

    void updatePosition(GLFWwindow* window) {
        if(xOffset >= 1.0f && movingRight) {
            movingRight = false;
        }
        if(xOffset <= -1.0f && !movingRight) {
            movingRight = true;
        }
        if(movingRight) {
            xOffset += speed;
        }
        else {
            xOffset -= speed;
        }
    }

    void onCollision(DynamicEntity* other) {
        movingRight = !movingRight;
    }
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void checkAllCollisions(DynamicEntity* one, std::vector<DynamicEntity*>& entities);
GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
GLfloat* newRectangle();

int main(int argc, char* argv[]) {
    
    Window win;
    win.init();

    std::vector<DynamicEntity*> entities;

    GLuint rectDataSize = 18;
    GLfloat* playerData = newRectangle();
    GLfloat* hEnemyData = newRectangle();
    GLfloat* vEnemyData = newRectangle();

    Player player(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-0.9f, 0.05f, 0.0f), playerData, rectDataSize);
    VerticalEnemy vertEnemy(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-0.0f, 0.0f, 0.0f), hEnemyData, rectDataSize);
    HorizontalEnemy horizEnemy(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.0f), vEnemyData, rectDataSize);

    entities.push_back(&player);
    entities.push_back(&vertEnemy);
    entities.push_back(&horizEnemy);

    glfwSetKeyCallback(win.win, keyCallback);
    GLuint programId = loadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
    GLint translationMatrixLocation = glGetUniformLocation(programId, "translationMatrix");
    GLint inputColorLocation = glGetUniformLocation(programId, "inputColor");

    // Set up arena
    GLuint arenaVertexArrayId;
    glGenVertexArrays(1, &arenaVertexArrayId);
    glBindVertexArray(arenaVertexArrayId);

    GLuint arenaVertexBuffer;
    glGenBuffers(1, &arenaVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, arenaVertexBuffer);

    GLfloat arenaVertexData[] = {
        -0.75f, 1.0f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        0.75f, 0.95f, 0.0f,

        -0.75f, -1.0f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f,

        -0.75f, 0.95f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        -0.7f, -0.95f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        -0.7f, 0.95f, 0.0f,
        -0.7f, -0.95f, 0.0f,

        0.75f, 0.95f, 0.0f,
        0.75f, -0.95f, 0.0f,
        0.7f, -0.95f, 0.0f,
        0.7f, 0.95f, 0.0f,
        0.75f, 0.95f, 0.0f,
        0.7f, -0.95f, 0.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(arenaVertexData), arenaVertexData, GL_STATIC_DRAW);

    //GLushort arenaIndexData[] = {
    //    0, 1, 2,  // Top wall
    //    1, 2, 3,
    //    4, 5, 6,  // Bottom wall
    //    5, 6, 7
    //};

    /*GLuint arenaElementBuffer;
    glGenBuffers(1, &arenaElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arenaElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arenaIndexData), arenaIndexData, GL_STATIC_DRAW);*/

    while(!glfwWindowShouldClose(win.win)) {

        glfwGetWindowSize(win.win, &win.width, &win.height);
        glViewport(0, 0, win.width, win.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);

        // Update enemies and player
        for(auto* entity : entities) {
            checkAllCollisions(entity, entities);
            entity->updateUniforms(translationMatrixLocation, inputColorLocation);
            entity->updatePosition(win.win);
            entity->draw();
        }

        glm::mat4 transMatrix = glm::translate(0.0f, 0.0f, 0.0f);
        glm::vec4 inputColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, arenaVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glDisableVertexAttribArray(0);

        glfwPollEvents();
        glfwSwapBuffers(win.win);
    }

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

void checkAllCollisions(DynamicEntity* one, std::vector<DynamicEntity*>& entities) {
    for(auto* two : entities) {
        if(&(*one) == &(*two)) {
            continue;
        }
        else {
            if(one->checkCollisions(two)) {
                one->onCollision(two);
            }
        }
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

GLfloat* newRectangle() {
    GLfloat* rect = new GLfloat[18] {
        -0.1f, 0.1f, 0.0f,
            0.0f, 0.1f, 0.0f,
            -0.1f, 0.0f, 0.0f,
            0.0f, 0.1f, 0.0f,
            0.0f, 0.0f, 0.0f,
            -0.1f, 0.0f, 0.0f,
    };

    return rect;
}