// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

#define RACKET_LENGTH 0.2
#define RACKET_POSITION_Y -0.6
#define BLOCK_LENGTH 0.03

Quad ball;
Quad racket;
Quad leftBlock;
Quad topBlock;
Quad rightBlock;
Quad gameOver;

float racketPosition; // moves only in a line, no need for y value

float ballPosition[2];
float ballSpeed[2];

bool isGameOver = true;
float timeGameOver = -100;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    ball.Init("ball.tga");

    racket.Init("block.tga");
    leftBlock.Init("block.tga");
    topBlock.Init("block.tga");
    rightBlock.Init("block.tga");

    gameOver.Init("gameOver.tga");

    srand (time(NULL));
}

void Movements() {
    if (isGameOver && glfwGetTime()-timeGameOver > 2) {

        ballPosition[0] = (rand() % 100) / 100.0 - 0.5;
        ballPosition[1] = 0;

        cout << ballPosition[0];

        ballSpeed[0] = ((rand() % 2)*2 - 1) * 0.00015;
        ballSpeed[1] = 0.00015;

        racketPosition = 0;

        isGameOver = false;
    }

    if (ballPosition[0] > 1-BLOCK_LENGTH || ballPosition[0] < -1+BLOCK_LENGTH) {
        ballSpeed[0] = -ballSpeed[0];
    }

    if (ballPosition[1] > 1-BLOCK_LENGTH|| (ballPosition[1] < RACKET_POSITION_Y+BLOCK_LENGTH && ballPosition[1] > RACKET_POSITION_Y && ballPosition[0] > racketPosition - RACKET_LENGTH && ballPosition[0] < racketPosition + RACKET_LENGTH)) {
        ballSpeed[1] = -ballSpeed[1];
    }

    if (ballPosition[1] < RACKET_POSITION_Y && !isGameOver) {
        isGameOver = true;
        timeGameOver = glfwGetTime();
    }

    ballPosition[0] += ballSpeed[0];
    ballPosition[1] += ballSpeed[1];
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    const float txBall = ballPosition[0];
    const float tyBall = ballPosition[1];
    const float sxBall = 0.05;
    const float syBall = 0.05;
    glm::mat4 IBall = glm::mat4(1.0f);
    glm::mat4 TBall = glm::translate(IBall, glm::vec3(txBall, tyBall, 0));
    glm::mat4 SBall = glm::scale(TBall, glm::vec3(sxBall, syBall, 0));
    glm::mat4 modelBall = SBall;
    ball.Draw(modelBall);

    const float txRacket = racketPosition;
    const float tyRacket = RACKET_POSITION_Y;
    const float sxRacket = RACKET_LENGTH;
    const float syRacket = BLOCK_LENGTH;
    glm::mat4 IRacket = glm::mat4(1.0f);
    glm::mat4 TRacket = glm::translate(IRacket, glm::vec3(txRacket, tyRacket, 0));
    glm::mat4 SRacket = glm::scale(TRacket, glm::vec3(sxRacket, syRacket, 0));
    glm::mat4 modelRacket = SRacket;
    racket.Draw(modelRacket);

    const float txLeftBlock = -1;
    const float tyLeftBlock = 0;
    const float sxLeftBlock = BLOCK_LENGTH;
    const float syLeftBlock = 1;
    glm::mat4 ILeftBlock = glm::mat4(1.0f);
    glm::mat4 TLeftBlock = glm::translate(ILeftBlock, glm::vec3(txLeftBlock, tyLeftBlock, 0));
    glm::mat4 SLeftBlock = glm::scale(TLeftBlock, glm::vec3(sxLeftBlock, syLeftBlock, 0));
    glm::mat4 modelLeftBlock = SLeftBlock;
    leftBlock.Draw(modelLeftBlock);

    const float txTopBlock = 0;
    const float tyTopBlock = 1;
    const float sxTopBlock = 1;
    const float syTopBlock = BLOCK_LENGTH;
    glm::mat4 ITopBlock = glm::mat4(1.0f);
    glm::mat4 TTopBlock = glm::translate(ITopBlock, glm::vec3(txTopBlock, tyTopBlock, 0));
    glm::mat4 STopBlock = glm::scale(TTopBlock, glm::vec3(sxTopBlock, syTopBlock, 0));
    glm::mat4 modelTopBlock = STopBlock;
    topBlock.Draw(modelTopBlock);

    const float txRightBlock = 1;
    const float tyRightBlock = 0;
    const float sxRightBlock = BLOCK_LENGTH;
    const float syRightBlock = 1;
    glm::mat4 IRightBlock = glm::mat4(1.0f);
    glm::mat4 TRightBlock = glm::translate(IRightBlock, glm::vec3(txRightBlock, tyRightBlock, 0));
    glm::mat4 SRightBlock = glm::scale(TRightBlock, glm::vec3(sxRightBlock, syRightBlock, 0));
    glm::mat4 modelRightBlock = SRightBlock;
    rightBlock.Draw(modelRightBlock);

    if (isGameOver) {
        const float txGameOver = 0;
        const float tyGameOver = 0;
        const float sxGameOver = 0.5;
        const float syGameOver = 0.5;
        glm::mat4 IGameOver = glm::mat4(1.0f);
        glm::mat4 TGameOver = glm::translate(IGameOver, glm::vec3(txGameOver, tyGameOver, 0));
        glm::mat4 SGameOver = glm::scale(TGameOver, glm::vec3(sxGameOver, syGameOver, 0));
        glm::mat4 modelGameOver = SGameOver;
        gameOver.Draw(modelGameOver);
    }

}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (!isGameOver && key == GLFW_KEY_RIGHT && racketPosition < 1-RACKET_LENGTH){
        racketPosition += 0.04;
    }
    else if (!isGameOver && key == GLFW_KEY_LEFT && racketPosition > -1+RACKET_LENGTH){
        racketPosition -= 0.04;
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "arkanoid", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Movements();
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ball.Cleanup();
    racket.Cleanup();
    leftBlock.Cleanup();
    topBlock.Cleanup();
    rightBlock.Cleanup();
    gameOver.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
