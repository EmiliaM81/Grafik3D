#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "particles.h"
#include "shader.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef SHADER_DIR
#define SHADER_DIR "shaders"
#endif

namespace {

int gFbW = 1280;
int gFbH = 720;
float gMass = 1.0f;
float gTimeScale = 1.0f;
int gQuality = 2;
int gDoppler = 1;
bool gShowParts = true;
OrbitCamera gCam;
ParticleSystem gParts;

bool gDragging = false;
double gLastX = 0.0;
double gLastY = 0.0;

void framebufferSizeCallback(GLFWwindow*, int w, int h) {
  gFbW = w;
  gFbH = h;
  glViewport(0, 0, w, h);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  if (action != GLFW_PRESS && action != GLFW_REPEAT) {
    return;
  }

  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {
    gMass = std::min(2.5f, gMass + 0.05f);
    std::cout << "masa / soczewka: " << gMass << "\n";
  } else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
    gMass = std::max(0.4f, gMass - 0.05f);
    std::cout << "masa / soczewka: " << gMass << "\n";
  } else if (key == GLFW_KEY_RIGHT_BRACKET) {
    gTimeScale = std::min(4.0f, gTimeScale + 0.1f);
    std::cout << "predkosc czasu: " << gTimeScale << "\n";
  } else if (key == GLFW_KEY_LEFT_BRACKET) {
    gTimeScale = std::max(0.0f, gTimeScale - 0.1f);
    std::cout << "predkosc czasu: " << gTimeScale << "\n";
  } else if (key == GLFW_KEY_1) {
    gQuality = 1;
    std::cout << "jakosc: 1 (lekka)\n";
  } else if (key == GLFW_KEY_2) {
    gQuality = 2;
    std::cout << "jakosc: 2\n";
  } else if (key == GLFW_KEY_3) {
    gQuality = 3;
    std::cout << "jakosc: 3 (wiecej krokow)\n";
  } else if (key == GLFW_KEY_D) {
    gDoppler = 1 - gDoppler;
    std::cout << "doppler: " << (gDoppler ? "on" : "off") << "\n";
  } else if (key == GLFW_KEY_P) {
    gShowParts = !gShowParts;
    std::cout << "czastki: " << (gShowParts ? "on" : "off") << "\n";
  } else if (key == GLFW_KEY_R) {
    gMass = 1.0f;
    gTimeScale = 1.0f;
    gQuality = 2;
    gDoppler = 1;
    gShowParts = true;
    gCam.reset();
    gParts.reset(gMass);
    std::cout << "reset kamery i parametrow\n";
  }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
  if (button != GLFW_MOUSE_BUTTON_LEFT) {
    return;
  }
  if (action == GLFW_PRESS) {
    gDragging = true;
    glfwGetCursorPos(window, &gLastX, &gLastY);
  } else if (action == GLFW_RELEASE) {
    gDragging = false;
  }
}

void cursorPosCallback(GLFWwindow*, double x, double y) {
  if (!gDragging) {
    return;
  }
  gCam.orbit(static_cast<float>(x - gLastX), static_cast<float>(y - gLastY));
  gLastX = x;
  gLastY = y;
}

void scrollCallback(GLFWwindow*, double, double yOffset) {
  gCam.zoom(static_cast<float>(yOffset));
}

// VAO z dwoma trojkatami = fullscreen quad
unsigned int makeFullscreenQuad() {
  const float verts[] = {
      -1.f, -1.f,
       1.f, -1.f,
       1.f,  1.f,
      -1.f, -1.f,
       1.f,  1.f,
      -1.f,  1.f,
  };

  unsigned int vao = 0, vbo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glBindVertexArray(0);
  return vao;
}

}  // namespace

int main() {
  if (!glfwInit()) {
    std::cerr << "glfwInit failed\n";
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(gFbW, gFbH, "czarna dziura", nullptr, nullptr);
  if (!window) {
    std::cerr << "nie dalo sie stworzyc okna GLFW\n";
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "gladLoadGLLoader failed\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
  std::cout << "LMB orbit, scroll zoom, +/- masa, [/] czas\n";
  std::cout << "1/2/3 jakosc, D doppler, P czastki, R reset, Esc\n";

  try {
    const std::string vert = std::string(SHADER_DIR) + "/fullscreen.vert";
    const std::string frag = std::string(SHADER_DIR) + "/blackhole.frag";
    Shader shader(vert, frag);
    const unsigned int quad = makeFullscreenQuad();

    gParts.reset(gMass);

    float simTime = 0.f;
    float last = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
      const float now = static_cast<float>(glfwGetTime());
      const float dt = now - last;
      last = now;
      simTime += dt * gTimeScale;

      gParts.update(dt * gTimeScale, gMass);

      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      const glm::vec3 eye = gCam.position();

      shader.use();
      shader.setFloat("uTime", simTime);
      shader.setVec2("uResolution", static_cast<float>(gFbW), static_cast<float>(gFbH));
      shader.setFloat("uMass", gMass);
      shader.setVec3("uCamPos", eye.x, eye.y, eye.z);
      shader.setInt("uQuality", gQuality);
      shader.setInt("uDoppler", gDoppler);
      shader.setInt("uShowParts", gShowParts ? 1 : 0);
      shader.setInt("uPartCount", static_cast<int>(gParts.count()));
      for (std::size_t i = 0; i < kMaxParticles; ++i) {
        const char* names[] = {
            "uParticles[0]", "uParticles[1]", "uParticles[2]", "uParticles[3]",
            "uParticles[4]", "uParticles[5]", "uParticles[6]", "uParticles[7]",
        };
        const glm::vec3& p = gParts.position(i);
        shader.setVec3(names[i], p.x, p.y, p.z);
      }

      glBindVertexArray(quad);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quad);
  } catch (const std::exception& e) {
    std::cerr << "blad: " << e.what() << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
