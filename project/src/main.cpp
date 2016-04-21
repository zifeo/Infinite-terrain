// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "framebuffer.h"

#include "grid/grid.h"
#include "normalTex/normalTex.h"
#include "perlinTex/perlinTex.h"

#include <map>
#include <stdint.h>

#define CHUNKS 2
#define DELTA 0.01
#define MAX_DIST 4294967296
#define VIEW_DIST 3 // in chunk

#define CAMERA_SPEED 0.04
#define MOUSE_SENSIBILTY 0.01

#ifndef M_PI
#define M_PI 3.14159268
#endif

using namespace glm;

bool arrows_down[4];
enum { UP, DOWN, RIGHT, LEFT };

int window_width = 1280;
int window_height = 720;
int tex_width = 1024;
int tex_height = 1024;
int nbFrames = 0;
float lastTime = 0;
float save_y = 0;

vec3 camPos = vec3(1, 1, 1);

float phi = 2.0f;
float theta = 0.0f;

float lastX = 0.0f, lastY = 0.0f;

// Perlin parameters
PerlinTex perlinTex;
int octave = 9;
float lac = 2;
float H = 1.25;
// Perlin parameters are ready to be used for normal and projection
bool perlin_ready = false;

typedef struct {
  FrameBuffer tex;
  GLuint perlinBuffer_tex_id;
  int x;
  int y;
} ChunkTex;

std::map<long long, ChunkTex> chunkMap;

// Normal texture
NormalTex normalTex;
FrameBuffer normalBuffer;

// Final grid vue
Grid grid;

// Trackball
// Helper matrices
mat4 projection_matrix;
mat4 view_matrix;
mat4 quad_model_matrix;

vec3 vecFromRot(float p, float t) {
  return vec3(sin(p) * cos(t), cos(p), sin(p) * sin(t));
}

void Init(GLFWwindow *window) {
  glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  // All texture, vues and framebuffer init
  perlinTex.Init();

  grid.Init();

  arrows_down[0] = arrows_down[1] = arrows_down[2] = arrows_down[3] = false;
  // resize_callback(window, window_width, window_height);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

long long getKey(int i, int j) {
  return ((i + 1000) % 1000) * 1000 + (j + 1000) % 1000;
}

void initChunk(int i, int j) {
  ChunkTex chunk;
  chunk.x = i;
  chunk.y = j;
  chunk.perlinBuffer_tex_id = chunk.tex.Init(tex_width, tex_height, true);
  chunk.tex.Bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  perlinTex.Draw(octave, lac, H, i - CHUNKS / 2, j - CHUNKS / 2);
  chunk.tex.Unbind();
  chunkMap.insert(std::pair<long long, ChunkTex>(getKey(i, j), chunk));
}

void frameBufferInit() {
  for (int i = 0; i < CHUNKS; i++) {
    for (int j = 0; j < CHUNKS; j++) {
      // initChunk(i, j);
    }
  }
}

void Display() {
  double currentTime = glfwGetTime();
  glViewport(0, 0, window_width, window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  view_matrix = lookAt(camPos, camPos + vecFromRot(phi, theta),
                       glm::vec3(0.0f, 1.0f, 0.0f));

  // cout << (camPos - glm::vec3(1.0f, 1.0f, 1.0f)).x << (camPos -
  // glm::vec3(1.0f, 1.0f, 1.0f)).y << (camPos - glm::vec3(1.0f, 1.0f, 1.0f)).z
  // << endl;

  if (perlin_ready) {

    glViewport(0, 0, window_width, window_height);
    for (std::map<long long, ChunkTex>::iterator it = chunkMap.begin();
         it != chunkMap.end(); it++) {
      int i = it->second.x;
      int j = it->second.y;
      mat4 model =
          quad_model_matrix *
          translate(IDENTITY_MATRIX, vec3((i - CHUNKS / 2) * 2 - DELTA * i, 0,
                                          -((j - CHUNKS / 2) * 2 - DELTA * j)));
      grid.Draw(it->second.perlinBuffer_tex_id, currentTime, model, view_matrix,
                projection_matrix);
    }
  } else {
    perlinTex.Draw(octave, lac, H, 0, 0);
  }

  // Measure speed
  nbFrames++;
  if (currentTime - lastTime >= 1.0 ||
      lastTime == 0) { // If last prinf() was more than 1 sec ago
    // printf and reset timer
    cout << nbFrames << " frames" << endl;
    nbFrames = 0;
    lastTime = currentTime;
  }

  // Camera movements
  if (arrows_down[UP]) {
    camPos += vecFromRot(phi, theta) * vec3(CAMERA_SPEED);
  }
  if (arrows_down[DOWN]) {
    camPos -= vecFromRot(phi, theta) * vec3(CAMERA_SPEED);
  }
  if (arrows_down[RIGHT]) {
    camPos -= cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, theta)) *
              vec3(CAMERA_SPEED);
  }
  if (arrows_down[LEFT]) {
    camPos += cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, theta)) *
              vec3(CAMERA_SPEED);
  }

  // Do we need to add chunks ?
  float camX = camPos.x > 0 ? (camPos.x + 1) / 2 : (camPos.x + 1) / 2 - 1;
  float camY = camPos.z > 0 ? (camPos.z + 1) / 2 : (camPos.z + 1) / 2 - 1;
  // cout << "-------------------------" << endl;
  // cout << camX << " and " << camY << endl;
  for (int dx = -VIEW_DIST; dx <= VIEW_DIST; dx++) {
    for (int dy = -VIEW_DIST; dy <= VIEW_DIST; dy++) {
      float i = camX + dx + 1;
      float j = -camY + dy + 1;

      // cout << i << " - " << j << endl;

      float fdx = dx + (camPos.x + 1) / 2 - (int)((camPos.x + 1) / 2);
      float fdy = dy + (camPos.z + 1) / 2 - (int)((camPos.z + 1) / 2);

      if (dx * dx + dy * dy <= VIEW_DIST * VIEW_DIST) {
        std::map<long long, ChunkTex>::iterator it =
            chunkMap.find(getKey(i, j));
        if (it == chunkMap.end()) { // no element at this position
          initChunk(i, j);
          cout << "Added chunk " << i << "-" << j << endl;
        }
      }
    }
  }
  // cout << "-------------------------" << endl;

  // Do we need to remove chunks ?
  std::vector<long long> toBeRemoved;
  for (std::map<long long, ChunkTex>::iterator it = chunkMap.begin();
       it != chunkMap.end(); it++) {
    int i = it->second.x;
    int j = it->second.y;

    float dx = -camX - 1 + i;
    float dy = camY - 1 + j;

    if (dx * dx + dy * dy > VIEW_DIST * VIEW_DIST * 1.1) {
      it->second.tex.Cleanup();
      toBeRemoved.push_back(it->first);
      cout << "Removed chunk " << i << "-" << j << endl;
    }
  }

  for (unsigned int i = 0; i < toBeRemoved.size(); i++) {
    chunkMap.erase(toBeRemoved[i]);
  }
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow *window, int x, int y) {
  // the framebuffer and the window doesn't necessarily have the same size
  // i.e. hidpi screens. so we need to get the correct one
  int width;
  int height;
  glfwGetWindowSize(window, &width, &height);
  return vec2(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
}

void MousePos(GLFWwindow *window, double x, double y) {
  // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

  int scaleFactor = 2;
  // glfwGetFramebufferSize(window, &window_width, &window_height);
  // glfwGetWindowSize(window, &window_width, &window_height);

  int diffy = (int)y - window_height / scaleFactor;
  int diffx = (int)x - window_width / scaleFactor;
  cout << diffx << ":" << diffy << endl;

  phi += diffy * MOUSE_SENSIBILTY;
  phi = clamp(phi, (float)M_PI / 10, 9 * (float)M_PI / 10);
  theta += diffx * MOUSE_SENSIBILTY;

  if (x != window_width / scaleFactor || y != window_height / scaleFactor) {
    glfwSetCursorPos(window, window_width / scaleFactor,
                     window_height / scaleFactor);
  }

  // cout << window_height / scaleFactor << "x" << window_width / scaleFactor <<
  // endl;
  // cout << window_height << "x" << window_width << endl;
  cout << x << ":" << y << endl;
  //}
}

// Gets called when the windows/framebuffer is resized.
void resize_callback(GLFWwindow *window, int width, int height) {
  window_width = width;
  window_height = height;
  cout << "=>" << window_height << "x" << window_width << endl;

  float ratio = window_width / (float)window_height;
  projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
  glViewport(0, 0, window_width, window_height);
}

void ErrorCallback(int error, const char *description) {
  fputs(description, stderr);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
    octave = key - GLFW_KEY_0;
    cout << "nbr octave : " << octave << endl;
  } else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    H += 0.01;
    H = H > 2 ? 2 : H;
    cout << "new H" << H << endl;
  } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    H -= 0.01;
    H = H < -2 ? -2 : H;
    cout << "new H" << H << endl;
  } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    lac += 0.01;
    lac = lac > 10 ? 10 : lac;
    cout << "new lac" << lac << endl;
  } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    lac -= 0.05;
    lac = lac < 0 ? 0 : lac;
    cout << "new lac" << lac << endl;
  } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    perlin_ready = !perlin_ready;
  } else if (key == GLFW_KEY_UP) {
    arrows_down[UP] = (action != GLFW_RELEASE);
  } else if (key == GLFW_KEY_DOWN) {
    arrows_down[DOWN] = (action != GLFW_RELEASE);
  } else if (key == GLFW_KEY_RIGHT) {
    arrows_down[RIGHT] = (action != GLFW_RELEASE);
  } else if (key == GLFW_KEY_LEFT) {
    arrows_down[LEFT] = (action != GLFW_RELEASE);
  }
}

int main(int argc, char *argv[]) {
  /// GLFW Initialization
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return EXIT_FAILURE;
  }
  glfwSetErrorCallback(ErrorCallback);

  /// Hint GLFW that we would like an OpenGL 3 context (at least)
  /// http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /// Attempt to open the window: fails if required version unavailable
  /// @note some Intel GPUs do not support OpenGL 3.2
  /// @note update the driver of your graphic card
  GLFWwindow *window = glfwCreateWindow(window_width, window_height,
                                        "INFINITE TERRAIN", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  /// Link the current context to the window we created
  glfwMakeContextCurrent(window);

  /// Set the callback for escape key
  glfwSetKeyCallback(window, KeyCallback);

  glfwSetWindowSizeCallback(window, resize_callback);

  // set the mouse press and position callback
  glfwSetCursorPosCallback(window, MousePos);

  /// GLEW Initialization (must have a context)
  glewExperimental = GL_TRUE; ///<
  if (glewInit() != GLEW_NO_ERROR) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    exit(EXIT_FAILURE);
  }

  std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
  Init(window);
  KeyCallback(window, GLFW_KEY_KP_1, 0, 0, 0);

  // update the window size with the framebuffer size (on hidpi screens the
  // framebuffer is bigger)
  glfwGetFramebufferSize(window, &window_width, &window_height);
  resize_callback(window, window_width, window_height);

  frameBufferInit();

  /// Render loop & keyboard input
  while (!glfwWindowShouldClose(window)) {
    Display();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  grid.Cleanup();
  normalTex.Cleanup();
  perlinTex.Cleanup();
  normalBuffer.Cleanup();

  for (std::map<long long, ChunkTex>::iterator it = chunkMap.begin();
       it != chunkMap.end(); it++) {
    it->second.tex.Cleanup();
  }

  /// Close OpenGL window and terminate GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
