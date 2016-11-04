#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
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

struct Texture2d {

};

struct Entity {
    GLfloat *vertexData;
    GLuint dataSize;
    GLuint vertexArrayId;
    GLuint vertexBuffer;
    GLuint numTriangles;

    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    glm::vec3 color;

    bool isSolid;
    
    float rotation;

    float xOffset;
    float yOffset;
    float xSize;
    float ySize;
    float x;
    float y;
    float coordinateXsize;
    float coordinateYsize;
    Point screenCoordinates;
    
    glm::mat4 transMatrix;
    glm::vec3 inputColor;
    enum entityType {PLAYER, ENEMY, ARENA};
    entityType type;

    Entity():
        position(0, 0),
        size(1, 1),
        velocity(0.0f),
        color(1.0f),
        rotation(0.0f),
        isSolid(false) {

    }

    Entity(glm::vec2 startingPos, glm::vec2 entitySize, glm::vec3 entityColor, glm::vec2 entityVelocity) :
        position(startingPos),
        size(entitySize),
        color(entityColor),
        velocity(entityVelocity),
        xOffset(startingPos.x),
        yOffset(startingPos.y) {

    }

    void initVertexArray() {
        glGenVertexArrays(1, &vertexArrayId);
        glBindVertexArray(vertexArrayId);
    }

    void initBuffer() {
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* dataSize, vertexData, GL_DYNAMIC_DRAW);
    }

    void setStaticPosition(GLint translationMatrixLocation, GLint inputColorLocation, glm::vec3* pos) {
        x = pos->x;
        y = pos->y;
        screenCoordinates = glToScreenCoordinates(x, y);
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
        if(type != ARENA) {
            screenCoordinates = glToScreenCoordinates(xOffset, yOffset);
        }
        transMatrix = glm::translate(xOffset, yOffset, 0.0f);
        inputColor = color;
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &transMatrix[0][0]);
        glUniform3fv(inputColorLocation, 1, &inputColor[0]);
    }

    virtual void draw(EntityRenderer &renderer) {
        renderer.drawEntity(position, size, rotation, color);
    }
};

struct DynamicEntity : Entity {

    DynamicEntity(glm::vec2 startingPos, glm::vec2 entitySize, glm::vec3 entityColor, glm::vec2 entityVelocity) :
        Entity(startingPos, entitySize, entityColor, entityVelocity) {}

    enum direction {UP, DOWN, LEFT, RIGHT};

    bool checkCollisions(Entity* other) {
        // Collision x-axis?
        bool collisionX = xOffset + xSize > other->xOffset &&
            other->xOffset + other->xSize > xOffset;
        // Collision y-axis?
        bool collisionY = yOffset + ySize > other->yOffset &&
            other->yOffset + other->ySize > yOffset;
        // Collision only if on both axes
        return collisionX && collisionY;
    }

    bool checkArenaCollision(Entity* other) {
        // Collision x-axis?
        bool collisionX = screenCoordinates.x + coordinateXsize > other->screenCoordinates.x &&
            other->screenCoordinates.x + other->coordinateXsize > screenCoordinates.x;
        // Collision y-axis?
        bool collisionY = screenCoordinates.y + coordinateYsize > other->screenCoordinates.y &&
            other->screenCoordinates.y + other->coordinateYsize > screenCoordinates.y;
        // Collision only if on both axes
        return collisionX && collisionY;
    }

    virtual void updatePosition(GLFWwindow* window, std::vector<Entity*>& walls) = 0;
    virtual void onCollision(Entity* other) = 0;
};

struct Player : DynamicEntity {
    bool dead = false;
    int lives = 3;
    float speed = 0.01f;
    direction dir;

    Player(glm::vec2 startingPos, glm::vec2 entitySize, glm::vec3 entityColor, glm::vec2 entityVelocity) :
        DynamicEntity(startingPos, entitySize, entityColor, entityVelocity) {}

    void updatePosition(GLFWwindow* window, std::vector<Entity*>& walls) {
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            dir = UP;
            if(screenCoordinates.y >= 0) {
                yOffset += speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            dir = DOWN;
            if(screenCoordinates.y + coordinateYsize <= WINDOW_HEIGHT) {
                yOffset -= speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            dir = LEFT;
            if(screenCoordinates.x >= 0) {
                xOffset -= speed;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            dir = RIGHT;
            if(screenCoordinates.x + coordinateXsize <= WINDOW_WIDTH) {
                xOffset += speed;
            }
        }
    }

    void onCollision(Entity* other) {
        float overlap;
        switch(other->type) {
            case ENEMY:
                dead = true;
                lives--;
                xOffset = position.x;
                yOffset = position.y;
                break;
            case ARENA:
                std::cout << "AAAAAAAAAAAAAA" << std::endl;
                switch(dir) {
                    case UP:
                        overlap = (other->screenCoordinates.y + other->coordinateYsize) - screenCoordinates.y;
                        yOffset -= overlap / WINDOW_HEIGHT;
                        break;
                    case DOWN:
                        overlap = (screenCoordinates.y + coordinateYsize) - other->screenCoordinates.y;
                        yOffset += overlap / WINDOW_HEIGHT;
                        break;
                    case LEFT:
                        overlap = (other->screenCoordinates.x + other->coordinateXsize) - screenCoordinates.x;
                        xOffset += overlap / WINDOW_HEIGHT;
                        break;
                    case RIGHT:
                        overlap = (screenCoordinates.x + coordinateXsize) - other->screenCoordinates.x;
                        xOffset -= overlap / WINDOW_HEIGHT;
                        break;
                }
                break;
        }
    }
};

struct VerticalEnemy : DynamicEntity {
    bool movingUp = true;
    float speed = ENEMY_SPEED;

    VerticalEnemy(glm::vec2 startingPos, glm::vec2 entitySize, glm::vec3 entityColor, glm::vec2 entityVelocity) :
        DynamicEntity(startingPos, entitySize, entityColor, entityVelocity) {}

    void updatePosition(GLFWwindow* window, std::vector<Entity*>& walls) {
        if(yOffset >= 1.0f && movingUp) {
            movingUp = false;
        }
        if(yOffset - ySize <= -1.0f && !movingUp) {
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
        float overlap;
        switch(other->type) {
            case ARENA:
                if(movingUp) {
                    overlap = overlap = (other->screenCoordinates.y + other->coordinateYsize) - screenCoordinates.y;
                    yOffset -= overlap / WINDOW_HEIGHT;
                }
                else {
                    overlap = (screenCoordinates.y + coordinateYsize) - other->screenCoordinates.y;
                    yOffset += overlap / WINDOW_HEIGHT;
                }
            case PLAYER:
            case ENEMY:
                movingUp = !movingUp;
                break;
        }
    }
};

struct HorizontalEnemy : DynamicEntity {
    bool movingRight = true;
    float speed = ENEMY_SPEED;

    HorizontalEnemy(glm::vec2 startingPos, glm::vec2 entitySize, glm::vec3 entityColor, glm::vec2 entityVelocity) :
        DynamicEntity(startingPos, entitySize, entityColor, entityVelocity) {}

    void updatePosition(GLFWwindow* window, std::vector<Entity*>& walls) {
        if(xOffset + xSize >= 1.0f && movingRight) {
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
        float overlap;
        switch(other->type) {
            case ARENA:
                if(movingRight) {
                    overlap = (screenCoordinates.x + coordinateXsize) - other->screenCoordinates.x;
                    xOffset -= overlap / WINDOW_HEIGHT;
                }
                else {
                    overlap = (other->screenCoordinates.x + other->coordinateXsize) - screenCoordinates.x;
                    xOffset += overlap / WINDOW_HEIGHT;
                }
            case PLAYER:
            case ENEMY:
                movingRight = !movingRight;
                break;
        }
    }
};

struct Shader {
    GLuint id;

    Shader() {}

    Shader& use() {
        glUseProgram(id);
        return *this;
    }

    void compile(const GLchar *vertexSource, const GLchar *fragmentSource) {
        GLuint sVertex, sFragment;

        sVertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(sVertex, 1, &vertexSource, NULL);
        glCompileShader(sVertex);
        checkCompileErrors(sVertex, "VERTEX");

        sFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(sFragment, 1, &fragmentSource, NULL);
        glCompileShader(sFragment);
        checkCompileErrors(sFragment, "FRAGMENT");

        id = glCreateProgram();
        glAttachShader(id, sVertex);
        glAttachShader(id, sFragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");

        glDeleteShader(sVertex);
        glDeleteShader(sFragment);
    }

    void checkCompileErrors(GLuint object, std::string type) {

    }

    void setVector3f(const GLchar *name, const glm::vec3 &value, bool useShader = false) {
        if(useShader)
            this->use();
        glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
    }

    void setMatrix4(const GLchar *name, const glm::mat4 &matrix, bool useShader = false) {
        if(useShader)
            this->use();
        glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
    }
};

struct EntityRenderer {
    Shader shader;
    GLuint quadVAO;
    
    EntityRenderer(Shader &shader) {
        this->shader = shader;
        this->initRenderData();
    }

    ~EntityRenderer() {
        glDeleteVertexArrays(1, &quadVAO);
    }

    void initRenderData() {
        GLuint vbo;
        GLfloat vertices[] = {
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,

            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void drawEntity(glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color) {
        shader.use();
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
        model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
        model = glm::scale(model, glm::vec3(size, 1.0f));

        shader.setMatrix4("model", model);
        shader.setVector3f("entityColor", color);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
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

    Player player(glm::vec2(-0.9f, 0.05f), glm::vec2(10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
    VerticalEnemy vertEnemy(glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f) ,glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f));
    HorizontalEnemy horizEnemy(glm::vec2(0.5f, -0.5f), glm::vec2(10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f));

    Entity arenaTop(glm::vec2(150.0f, 0.0f), glm::vec2(900.0f, 20.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f));
    Entity arenaBottom(glm::vec2(150.0f, 780.0f), glm::vec2(900.0f, 20.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f));
    Entity arenaRight(glm::vec2(1020.0f, 20.0f), glm::vec2(20.0f, 760.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f));

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
            entity->updatePosition(win.win, arenaWalls);
            entity->draw();
        }

        arenaTop.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(-0.75f, 1.0f, 0.0f));
        arenaTop.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaTop.draw();

        arenaBottom.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(-0.75f, -1.0f, 0.0f));
        arenaBottom.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaBottom.draw();

        arenaRight.setStaticPosition(translationMatrixLocation, inputColorLocation, &glm::vec3(0.7f, 0.95f, 0.0f));
        arenaRight.updateUniforms(translationMatrixLocation, inputColorLocation);
        arenaRight.draw();

        /*std::cout << "TopWall.x: " << arenaTop.screenCoordinates.x << std::endl;
        std::cout << "TopWall.y: " << arenaTop.screenCoordinates.y << std::endl;
        std::cout << "TopWall.y.size: " << arenaTop.screenCoordinates.y + arenaTop.coordinateYsize << std::endl;*/
        // screenCoordinates = glToScreenCoordinates(arenaRight.x, arenaRight.y);
        // std::cout << "Right.x: " << screenCoordinates.x << std::endl;
        // std::cout << "Right.y: " << screenCoordinates.y << std::endl;

        /*std::cout << "BottomWall.x: " << arenaBottom.screenCoordinates.x << std::endl;
        std::cout << "BottomWall.y: " << arenaBottom.screenCoordinates.y << std::endl;
        std::cout << "BottomWall.y.size: " << arenaBottom.screenCoordinates.y + arenaBottom.coordinateYsize << std::endl;*/
        std::cout << "BottomWall.x: " << arenaBottom.x << std::endl;
        std::cout << "BottomWall.y: " << arenaBottom.y << std::endl;
        std::cout << "BottomWall.y.size: " << arenaBottom.y + arenaBottom.ySize << std::endl;

        std::cout << "Player.x: " << player.screenCoordinates.x << std::endl;
        std::cout << "Player.y: " << player.screenCoordinates.y << std::endl;
        std::cout << "Player.x.size: " << player.screenCoordinates.x + player.coordinateXsize << std::endl;
        std::cout << "Player.y.size: " << player.screenCoordinates.y + player.coordinateYsize << std::endl;

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
        if(de->checkArenaCollision(wall)) {
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