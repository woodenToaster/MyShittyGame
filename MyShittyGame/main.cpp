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
#define ARENA_VERTS 90
#define RECT_VERTS 18
#define DYNAMIC_ENTITY_X_SIZE 0.1f
#define DYMAMIC_ENTITY_Y_SIZE 0.1f
#define ENEMY_SPEED 0.02f

struct Window {
    int width;
    int height;
    const int windowWidth = WINDOW_WIDTH;
    const int windowHeight = WINDOW_HEIGHT;
    GLFWwindow* win;

    Window() {
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

    ~Window() {
        glfwTerminate();
    }

    static void error_callback(int e, const char *d) {
        printf("Error %d: %s\n", e, d);
    }
};

struct Point {
    float x;
    float y;
};
Point glToScreenCoordinates(float glX, float glY);

struct Entity {
    GLfloat *vertexData;
    GLuint size;
    GLuint vertexArrayId;
    GLuint vertexBuffer;
    GLuint numTriangles;
    float xOffset;
    float yOffset;
    float xSize;
    float ySize;
    float x;
    float y;
    float coordinateXsize;
    float coordinateYsize;
    Point screenCoordinates;
    glm::vec3 startingPosition;
    glm::vec3 color;
    glm::mat4 transMatrix;
    glm::vec3 inputColor;

    Entity(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength, float xLength, float yLength) :
        color(entityColor),
        inputColor(entityColor),
        xOffset(startingPos.x),
        yOffset(startingPos.y),
        x(startingPos.x),
        y(startingPos.y),
        startingPosition(startingPos),
        vertexData(data),
        size(dataLength),
        xSize(xLength),
        ySize(yLength),
        coordinateXsize((xSize * WINDOW_WIDTH) / 2),
        coordinateYsize((ySize * WINDOW_HEIGHT) / 2) {

        numTriangles = dataLength / 3;
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

    void setStaticPosition(GLint translationMatrixLocation, GLint inputColorLocation, glm::vec3* pos) {
        x = pos->x;
        y = pos->y;
        transMatrix = glm::translate(0.0f, 0.0f, 0.0f);
        inputColor = color;
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);
    }

    void updateUniforms(GLint translationMatrixLocation, GLint inputColorLocation, glm::vec3* translate = NULL) {
        if(translate) {
            xOffset = translate->x;
            yOffset = translate->y;
        }
        screenCoordinates = glToScreenCoordinates(xOffset, yOffset);
        transMatrix = glm::translate(xOffset, yOffset, 0.0f);
        inputColor = color;
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);
    }

    void draw() {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, numTriangles);
        glDisableVertexAttribArray(0);
    }
};

struct DynamicEntity : Entity {

    enum direction { UP, DOWN, LEFT, RIGHT };

    DynamicEntity(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength, float xLength, float yLength) :
        Entity(entityColor, startingPos,data, dataLength, xLength, yLength) {}

    bool checkCollisions(Entity* other) {
        // Collision x-axis?
        bool collisionX = xOffset + xSize >= other->xOffset &&
            other->xOffset + other->xSize >= xOffset;
        // Collision y-axis?
        bool collisionY = yOffset + ySize >= other->yOffset &&
            other->yOffset + other->ySize >= yOffset;

        // Collision only if on both axes
        return collisionX && collisionY;
    }

    bool checkArenaCollisions(Entity* other) {
        // Collision x-axis?
        bool collisionX = xOffset + xSize >= other->x &&
            other->x + other->xSize >= xOffset;
        // Collision y-axis?
        bool collisionY = abs(yOffset) + ySize >= other->y &&
            other->y + other->ySize >= yOffset;

        // Collision only if on both axes
        return collisionX && collisionY;
    }

    virtual void updatePosition(GLFWwindow* window) = 0;
    virtual void onCollision(Entity* other) = 0;
};

struct Player : DynamicEntity {
    bool dead = false;
    int lives = 3;
    float speed = 0.01f;

    Player(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength, DYNAMIC_ENTITY_X_SIZE, DYMAMIC_ENTITY_Y_SIZE) {}

    void updatePosition(GLFWwindow* window) {
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if(screenCoordinates.y >= 0) {
                yOffset += speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if(screenCoordinates.y + coordinateYsize <= WINDOW_HEIGHT) {
                yOffset -= speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if(screenCoordinates.x >= 0) {
                xOffset -= speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if(screenCoordinates.x + coordinateXsize <= WINDOW_WIDTH) {
                xOffset += speed;
            }
        }
    }

    void onCollision(Entity* other) {
        dead = true;
        lives--;
        xOffset = startingPosition.x;
        yOffset = startingPosition.y;
    }
};

struct VerticalEnemy : DynamicEntity {
    bool movingUp = true;
    float speed = ENEMY_SPEED;

    VerticalEnemy(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength, float xLength, float yLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength, xLength, yLength) {}

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

    void onCollision(Entity* other) {
        movingUp = !movingUp;
    }
};

struct HorizontalEnemy : DynamicEntity {
    bool movingRight = true;
    float speed = ENEMY_SPEED;

    HorizontalEnemy(glm::vec3 entityColor, glm::vec3 startingPos, float* data, GLuint dataLength, float xLength, float yLength) :
        DynamicEntity(entityColor, startingPos, data, dataLength, xLength, yLength) {}

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

    void onCollision(Entity* other) {
        movingRight = !movingRight;
    }
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double x, double y);

void checkAllCollisions(DynamicEntity* one, std::vector<DynamicEntity*>& entities);
void checkArenaCollisions(DynamicEntity* de, std::vector<Entity*> walls);
GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
GLfloat* newRectangle();
GLfloat* newVerticalArenaBar();
GLfloat* newHorizontalArenaBarTop();
GLfloat* newHorizaontalArenaBarBottom();
GLfloat* newHorizontalArenaBarShort();
GLfloat* newArena();

int main(int argc, char* argv[]) {

    Window win;

    std::vector<DynamicEntity*> entities;
    std::vector<Entity*> arenaWalls;

    GLfloat* playerData = newRectangle();
    GLfloat* hEnemyData = newRectangle();
    GLfloat* vEnemyData = newRectangle();
    
    GLfloat* arenaTopData = newHorizontalArenaBarTop();
    GLfloat* arenaRightData = newVerticalArenaBar();
    GLfloat* arenaBottomData = newHorizaontalArenaBarBottom();

    Player player(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-0.9f, 0.05f, 0.0f), playerData, RECT_VERTS);
    VerticalEnemy vertEnemy(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-0.0f, 0.0f, 0.0f), hEnemyData, RECT_VERTS, DYNAMIC_ENTITY_X_SIZE, DYMAMIC_ENTITY_Y_SIZE);
    HorizontalEnemy horizEnemy(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, -0.5f, 0.0f), vEnemyData, RECT_VERTS, DYNAMIC_ENTITY_X_SIZE, DYMAMIC_ENTITY_Y_SIZE);

    Entity arenaTop(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), arenaTopData, RECT_VERTS, 1.5f, 0.1f);
    Entity arenaBottom(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), arenaBottomData, RECT_VERTS, 1.5f, 0.1f);
    Entity arenaRight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), arenaRightData, RECT_VERTS, 0.05f, 2.0f);

    arenaWalls.push_back(&arenaTop);
    arenaWalls.push_back(&arenaBottom);
    arenaWalls.push_back(&arenaRight);

    entities.push_back(&player);
    entities.push_back(&vertEnemy);
    entities.push_back(&horizEnemy);

    glfwSetKeyCallback(win.win, keyCallback);
    glfwSetCursorPosCallback(win.win, cursorPosCallback);

    GLuint programId = loadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
    GLint translationMatrixLocation = glGetUniformLocation(programId, "translationMatrix");
    GLint inputColorLocation = glGetUniformLocation(programId, "inputColor");

    //arenaTop.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(-0.75f, 1.0f, 0.0f));

    while(!glfwWindowShouldClose(win.win)) {

        glfwGetWindowSize(win.win, &win.width, &win.height);
        glViewport(0, 0, win.width, win.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);

        // Update enemies and player
        for(auto* entity : entities) {
            checkAllCollisions(entity, entities);
            checkArenaCollisions(entity, arenaWalls);
            entity->updateUniforms(translationMatrixLocation, inputColorLocation);
            entity->updatePosition(win.win);
            entity->draw();
        }

        arenaTop.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(-0.75f, 1.0f, 0.0f));
        arenaTop.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaTop.draw();

        arenaBottom.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(-0.75f, -1.0f, 0.0f));
        arenaBottom.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaBottom.draw();

        Point screenCoordinates = glToScreenCoordinates(arenaTop.x, arenaTop.y);
        /*std::cout << "Top.x: " << screenCoordinates.x << std::endl;
        std::cout << "Top.y: " << screenCoordinates.y << std::endl;
        screenCoordinates = glToScreenCoordinates(arenaRight.x, arenaRight.y);
        std::cout << "Right.x: " << screenCoordinates.x << std::endl;
        std::cout << "Right.y: " << screenCoordinates.y << std::endl;*/
        
        screenCoordinates = glToScreenCoordinates(player.xOffset, player.yOffset);
        std::cout << "x: " << screenCoordinates.x << std::endl;
        std::cout << "y: " << screenCoordinates.y << std::endl;

        arenaRight.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(0.7f, 0.95f, 0.0f));
        arenaRight.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaRight.draw();

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

Point glToScreenCoordinates(float glX, float glY) {
    Point screenPoint;

    if(glX <= 0) {
        screenPoint.x = ((1 - abs(glX)) * WINDOW_WIDTH) / 2;
    }
    else if(glX > 0) {
        screenPoint.x = ((glX + 1) * WINDOW_WIDTH) / 2;
    }

    if(glY < 0) {
        screenPoint.y = ((1 - glY) * WINDOW_HEIGHT) / 2;
    }
    else if(glY >= 0) {
        screenPoint.y = (WINDOW_HEIGHT * (1 - glY)) / 2;
    }

    return screenPoint;
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    std::cout << "Mouse " << x << ", " << y << std::endl;
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

void checkArenaCollisions(DynamicEntity* de, std::vector<Entity*> walls) {
    for(auto* wall : walls) {
        if(de->checkArenaCollisions(wall)) {
            de->onCollision(wall);
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
        0.0f, 0.0f, 0.0f,
        0.0f, -0.1f, 0.0f,
        0.1f, 0.0f, 0.0f,
        0.1f, 0.0f, 0.0f,
        0.1f, -0.1f, 0.0f,
        0.0f, -0.1f, 0.0f,
    };

    return rect;
}

GLfloat* newVerticalArenaBar() {
    GLfloat* data = new GLfloat[RECT_VERTS] {
        0.75f, 0.95f, 0.0f,
        0.75f, -0.95f, 0.0f,
        0.7f, -0.95f, 0.0f,
        0.7f, 0.95f, 0.0f,
        0.75f, 0.95f, 0.0f,
        0.7f, -0.95f, 0.0f
    };
    return data;
}

GLfloat* newHorizontalArenaBarTop() {
    GLfloat* data = new GLfloat[RECT_VERTS] {
        -0.75f, 1.0f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        0.75f, 0.95f, 0.0f
    };
    return data;
}

GLfloat* newHorizaontalArenaBarBottom() {
    GLfloat* data = new GLfloat[RECT_VERTS] {
        -0.75f, -1.0f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f
    };
    return data;
}

GLfloat* newVerticalArenaBarShort() {
    GLfloat* data = new GLfloat[RECT_VERTS] {
        -0.75f, 0.95f, 0.0f,
        -0.75f, 0.3f, 0.0f,
        -0.7f, 0.95f, 0.0f,
        -0.75f, 0.3f, 0.0f,
        -0.7f, 0.3f, 0.0f,
        -0.7f, 0.95f, 0.0f,
    };
    return data;
}

GLfloat* newArena() {
    GLfloat* arenaVertexData = new GLfloat[ARENA_VERTS] {
        -0.75f, 1.0f, 0.0f, // Top
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        -0.75f, 0.95f, 0.0f,
        0.75f, 1.0f, 0.0f,
        0.75f, 0.95f, 0.0f,

        -0.75f, -1.0f, 0.0f, // Bottom
        -0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f,
        -0.75f, -0.95f, 0.0f,
        0.75f, -0.95f, 0.0f,
        0.75f, -1.0f, 0.0f,

        -0.75f, 0.95f, 0.0f, // Left top 
        -0.75f, 0.3f, 0.0f,
        -0.7f, 0.95f, 0.0f,
        -0.75f, 0.3f, 0.0f,
        -0.7f, 0.3f, 0.0f,
        -0.7f, 0.95f, 0.0f,

        -0.75f, -0.95f, 0.0f, // Left bottom
        -0.75f, -0.3f, 0.0f,
        -0.7f, -0.95f, 0.0f,
        -0.75f, -0.3f, 0.0f,
        -0.7f, -0.3f, 0.0f,
        -0.7f, -0.95f, 0.0f,

        0.75f, 0.95f, 0.0f, // Right
        0.75f, -0.95f, 0.0f,
        0.7f, -0.95f, 0.0f,
        0.7f, 0.95f, 0.0f,
        0.75f, 0.95f, 0.0f,
        0.7f, -0.95f, 0.0f
    };

    return arenaVertexData;
}