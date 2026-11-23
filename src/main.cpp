#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "hud.h"
#include "particles.h"
#include "shader.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef SHADER_DIR
#define SHADER_DIR "shaders"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

int gFbW = 1280;
int gFbH = 720;
float gMass = 1.0f;
float gTimeScale = 1.0f;
int gQuality = 2;
int gDoppler = 1;
bool gShowParts = true;
bool gPaused = false;
bool gAutoOrbit = false;
bool gShowHelp = false;
bool gLensing = true;
float gDiskRIn = 2.8f;
float gDiskTilt = 0.0f;
int gViewPreset = 0;
bool gWantShot = false;
bool gFullscreen = false;
int gWinX = 100, gWinY = 100, gWinW = 1280, gWinH = 720;

OrbitCamera gCam;
ParticleSystem gParts;
GLFWwindow* gWindow = nullptr;

bool gDragging = false;
double gLastX = 0.0;
double gLastY = 0.0;

void framebufferSizeCallback(GLFWwindow*, int w, int h) {
  gFbW = w;
  gFbH = h;
  glViewport(0, 0, w, h);
}

void applyViewPreset(int idx) {
  gViewPreset = (idx % 3 + 3) % 3;
  if (gViewPreset == 0) {
    gCam.setView(0.0f, static_cast<float>(M_PI) * 0.45f, 14.0f);
  } else if (gViewPreset == 1) {
    gCam.setView(0.0f, 0.05f, 12.0f);
  } else {
    gCam.setView(0.2f, 0.22f, 11.0f);
  }
}

void toggleFullscreen(GLFWwindow* window) {
  if (!gFullscreen) {
    glfwGetWindowPos(window, &gWinX, &gWinY);
    glfwGetWindowSize(window, &gWinW, &gWinH);
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);
    glfwSetWindowMonitor(window, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
    gFullscreen = true;
  } else {
    glfwSetWindowMonitor(window, nullptr, gWinX, gWinY, gWinW, gWinH, 0);
    gFullscreen = false;
  }
}

void savePpm(const char* path) {
  std::vector<unsigned char> px(static_cast<std::size_t>(gFbW) * static_cast<std::size_t>(gFbH) * 3);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, gFbW, gFbH, GL_RGB, GL_UNSIGNED_BYTE, px.data());

  std::ofstream out(path, std::ios::binary);
  if (!out) {
    std::cerr << "nie moge zapisac " << path << "\n";
    return;
  }
  out << "P6\n" << gFbW << " " << gFbH << "\n255\n";
  for (int y = gFbH - 1; y >= 0; --y) {
    const unsigned char* row = px.data() + static_cast<std::size_t>(y) * gFbW * 3;
    out.write(reinterpret_cast<const char*>(row), gFbW * 3);
  }
  std::cout << "screenshot: " << path << "\n";
}

void resetParams() {
  gMass = 1.0f;
  gTimeScale = 1.0f;
  gQuality = 2;
  gDoppler = 1;
  gShowParts = true;
  gPaused = false;
  gAutoOrbit = false;
  gShowHelp = false;
  gLensing = true;
  gDiskRIn = 2.8f;
  gDiskTilt = 0.0f;
  gViewPreset = 0;
  gCam.reset();
  gParts.reset(gMass);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  if (action != GLFW_PRESS && action != GLFW_REPEAT) {
    return;
  }
  const bool press = (action == GLFW_PRESS);

  if (key == GLFW_KEY_ESCAPE && press) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {
    gMass = std::min(2.5f, gMass + 0.05f);
  } else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
    gMass = std::max(0.4f, gMass - 0.05f);
  } else if (key == GLFW_KEY_RIGHT_BRACKET || key == GLFW_KEY_0) {
    gTimeScale = std::min(4.0f, gTimeScale + 0.1f);
  } else if (key == GLFW_KEY_LEFT_BRACKET || key == GLFW_KEY_9) {
    gTimeScale = std::max(0.0f, gTimeScale - 0.1f);
  } else if (key == GLFW_KEY_Q && press) {
    gQuality = gQuality >= 3 ? 1 : gQuality + 1;
  } else if (key == GLFW_KEY_D && press) {
    gDoppler = 1 - gDoppler;
  } else if (key == GLFW_KEY_P && press) {
    gShowParts = !gShowParts;
  } else if (key == GLFW_KEY_SPACE && press) {
    gPaused = !gPaused;
  } else if (key == GLFW_KEY_A && press) {
    gAutoOrbit = !gAutoOrbit;
  } else if (key == GLFW_KEY_H && press) {
    gShowHelp = !gShowHelp;
  } else if (key == GLFW_KEY_L && press) {
    gLensing = !gLensing;
  } else if (key == GLFW_KEY_V && press) {
    applyViewPreset(gViewPreset + 1);
  } else if (key == GLFW_KEY_COMMA) {
    if (gParts.count() > 0) {
      gParts.setCount(gParts.count() - 1, gMass);
    }
  } else if (key == GLFW_KEY_U) {
    gDiskRIn = std::max(1.6f, gDiskRIn - 0.15f);
  } else if (key == GLFW_KEY_I) {
    gDiskRIn = std::min(7.0f, gDiskRIn + 0.15f);
  } else if (key == GLFW_KEY_T) {
    gDiskTilt = std::clamp(gDiskTilt - 0.04f, -0.7f, 0.7f);
  } else if (key == GLFW_KEY_Y) {
    gDiskTilt = std::clamp(gDiskTilt + 0.04f, -0.7f, 0.7f);
  } else if (key == GLFW_KEY_F && press) {
    toggleFullscreen(window);
  } else if (key == GLFW_KEY_F12 && press) {
    gWantShot = true;
  } else if (key == GLFW_KEY_R && press) {
    resetParams();
  }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      gDragging = true;
      glfwGetCursorPos(window, &gLastX, &gLastY);
    } else if (action == GLFW_RELEASE) {
      gDragging = false;
    }
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    gParts.spawn(gMass);
    gShowParts = true;
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

std::string buildHudText(float simTime) {
  std::ostringstream ss;
  ss.setf(std::ios::fixed);
  ss.precision(2);
  ss << "masa " << gMass
     << "  t " << simTime
     << "  scale " << gTimeScale
     << (gPaused ? " PAUSE" : "")
     << "\n"
     << "jakosc " << gQuality
     << "  doppler " << (gDoppler ? "on" : "off")
     << "  lens " << (gLensing ? "on" : "off")
     << "\n"
     << "ciala test. " << (gShowParts ? "on" : "off")
     << " n=" << gParts.count()
     << "  rIn " << gDiskRIn
     << "  tilt " << gDiskTilt
     << (gAutoOrbit ? "  auto" : "")
     << "   H=help";

  if (gShowHelp) {
    ss << "\n\n"
       << "LMB orbit  scroll zoom\n"
       << "+- masa  9 0 czas  Spacja pauza  A auto\n"
       << "Q jakosc  D doppler  L lensing\n"
       << "P ciala testowe  ,  MMB wiecej cial testowych\n"
       << "U I rIn  T Y tilt  V ujecie\n"
       << "F full  F12 ppm  R reset  Esc";
  }
  return ss.str();
}

bool nearly(float a, float b, float eps = 0.001f) {
  return std::fabs(a - b) < eps;
}

int runLogicSelftest() {
  int fails = 0;
  auto expect = [&](bool ok, const char* name) {
    if (!ok) {
      std::cerr << "FAIL " << name << "\n";
      ++fails;
    } else {
      std::cout << "ok " << name << "\n";
    }
  };

  resetParams();
  keyCallback(nullptr, GLFW_KEY_EQUAL, 0, GLFW_PRESS, 0);
  expect(nearly(gMass, 1.05f), "masa +");
  keyCallback(nullptr, GLFW_KEY_MINUS, 0, GLFW_PRESS, 0);
  expect(nearly(gMass, 1.0f), "masa -");

  keyCallback(nullptr, GLFW_KEY_0, 0, GLFW_PRESS, 0);
  expect(nearly(gTimeScale, 1.1f), "czas 0");
  keyCallback(nullptr, GLFW_KEY_9, 0, GLFW_PRESS, 0);
  expect(nearly(gTimeScale, 1.0f), "czas 9");
  keyCallback(nullptr, GLFW_KEY_RIGHT_BRACKET, 0, GLFW_PRESS, 0);
  expect(nearly(gTimeScale, 1.1f), "czas ]");

  gQuality = 2;
  keyCallback(nullptr, GLFW_KEY_Q, 0, GLFW_PRESS, 0);
  expect(gQuality == 3, "jakosc Q->3");
  keyCallback(nullptr, GLFW_KEY_Q, 0, GLFW_PRESS, 0);
  expect(gQuality == 1, "jakosc Q->1");
  keyCallback(nullptr, GLFW_KEY_Q, 0, GLFW_PRESS, 0);
  expect(gQuality == 2, "jakosc Q->2");

  const int dop0 = gDoppler;
  keyCallback(nullptr, GLFW_KEY_D, 0, GLFW_PRESS, 0);
  expect(gDoppler == 1 - dop0, "doppler press");
  keyCallback(nullptr, GLFW_KEY_D, 0, GLFW_REPEAT, 0);
  expect(gDoppler == 1 - dop0, "doppler ignore repeat");

  keyCallback(nullptr, GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
  expect(gPaused, "pauza on");
  keyCallback(nullptr, GLFW_KEY_SPACE, 0, GLFW_REPEAT, 0);
  expect(gPaused, "pauza ignore repeat");
  keyCallback(nullptr, GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
  expect(!gPaused, "pauza off");

  keyCallback(nullptr, GLFW_KEY_A, 0, GLFW_PRESS, 0);
  expect(gAutoOrbit, "auto on");
  keyCallback(nullptr, GLFW_KEY_L, 0, GLFW_PRESS, 0);
  expect(!gLensing, "lensing off");
  keyCallback(nullptr, GLFW_KEY_H, 0, GLFW_PRESS, 0);
  expect(gShowHelp, "help on");

  keyCallback(nullptr, GLFW_KEY_P, 0, GLFW_PRESS, 0);
  expect(!gShowParts, "parts hide");
  keyCallback(nullptr, GLFW_KEY_P, 0, GLFW_PRESS, 0);
  expect(gShowParts, "parts show");

  resetParams();
  gParts.reset(gMass);
  expect(gParts.count() == 16, "parts reset 16");
  gParts.setCount(10, gMass);
  keyCallback(nullptr, GLFW_KEY_COMMA, 0, GLFW_PRESS, 0);
  expect(gParts.count() == 9, "parts comma");
  gParts.setCount(kMaxParticles, gMass);
  const std::size_t before = gParts.count();
  gParts.spawn(gMass);
  expect(gParts.count() == before, "spawn at max noop");
  gParts.setCount(3, gMass);
  gParts.spawn(gMass);
  expect(gParts.count() == 4, "spawn adds");

  const float yaw0 = gCam.yaw();
  keyCallback(nullptr, GLFW_KEY_V, 0, GLFW_PRESS, 0);
  expect(gViewPreset == 1, "preset 1");
  expect(!nearly(gCam.yaw(), yaw0) || !nearly(gCam.pitch(), 0.22f) ||
             !nearly(gCam.distance(), 11.f),
         "preset moves cam");
  keyCallback(nullptr, GLFW_KEY_V, 0, GLFW_REPEAT, 0);
  expect(gViewPreset == 1, "preset ignore repeat");

  keyCallback(nullptr, GLFW_KEY_I, 0, GLFW_PRESS, 0);
  expect(gDiskRIn > 2.8f, "rIn up");
  keyCallback(nullptr, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
  expect(gDiskTilt > 0.f, "tilt up");

  keyCallback(nullptr, GLFW_KEY_R, 0, GLFW_PRESS, 0);
  expect(nearly(gMass, 1.f) && nearly(gTimeScale, 1.f) && gLensing && !gPaused,
         "reset");

  if (fails == 0) {
    std::cout << "selftest logic: PASS\n";
    return 0;
  }
  std::cerr << "selftest logic: " << fails << " fails\n";
  return 1;
}

}

long long ppmDiff(const char* a, const char* b) {
  std::ifstream fa(a, std::ios::binary), fb(b, std::ios::binary);
  if (!fa || !fb) return -1;
  std::string mag1, mag2;
  int w1 = 0, h1 = 0, w2 = 0, h2 = 0, maxv1 = 0, maxv2 = 0;
  fa >> mag1 >> w1 >> h1 >> maxv1;
  fb >> mag2 >> w2 >> h2 >> maxv2;
  fa.get();
  fb.get();
  if (mag1 != "P6" || mag2 != "P6" || w1 != w2 || h1 != h2) return -1;
  long long diff = 0;
  const long long n = static_cast<long long>(w1) * h1 * 3;
  for (long long i = 0; i < n; ++i) {
    const int ca = fa.get();
    const int cb = fb.get();
    if (ca < 0 || cb < 0) return -1;
    diff += std::abs(ca - cb);
  }
  return diff;
}

int main(int argc, char** argv) {
  const bool logicOnly = (argc > 1 && std::string(argv[1]) == "--selftest");
  const bool guiTest = (argc > 1 && std::string(argv[1]) == "--guitest");

  if (logicOnly) {
    return runLogicSelftest();
  }

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
  gWindow = window;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(guiTest ? 0 : 1);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  if (!guiTest) {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
  }

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "gladLoadGLLoader failed\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  glfwGetFramebufferSize(window, &gFbW, &gFbH);
  glViewport(0, 0, gFbW, gFbH);

  std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
  if (!guiTest) {
    std::cout << "H = lista klawiszy, parametry na HUD\n";
  }

  int exitCode = 0;
  try {
    const std::string vert = std::string(SHADER_DIR) + "/fullscreen.vert";
    const std::string frag = std::string(SHADER_DIR) + "/blackhole.frag";
    Shader shader(vert, frag);
    Hud hud;
    const unsigned int quad = makeFullscreenQuad();

    gParts.reset(gMass);

    float simTime = 0.f;
    float last = static_cast<float>(glfwGetTime());
    int shotIdx = 1;
    int guiStep = 0;
    int warm = 0;

    while (!glfwWindowShouldClose(window)) {
      const float now = static_cast<float>(glfwGetTime());
      const float dt = now - last;
      last = now;

      if (guiTest) {
        if (warm < 3) {
          ++warm;
        } else if (guiStep == 0) {
          gLensing = true;
          gShowHelp = true;
          gWantShot = true;
          guiStep = 1;
        } else if (guiStep == 1 && !gWantShot) {
          keyCallback(window, GLFW_KEY_L, 0, GLFW_PRESS, 0);
          gWantShot = true;
          guiStep = 2;
        } else if (guiStep == 2 && !gWantShot) {
          keyCallback(window, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_Y, 0, GLFW_PRESS, 0);
          gWantShot = true;
          guiStep = 3;
        } else if (guiStep == 3 && !gWantShot) {
          keyCallback(window, GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_D, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_COMMA, 0, GLFW_PRESS, 0);
          keyCallback(window, GLFW_KEY_COMMA, 0, GLFW_PRESS, 0);
          gWantShot = true;
          guiStep = 4;
        } else if (guiStep == 4 && !gWantShot) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
      }

      const float simDt = gPaused ? 0.f : dt * gTimeScale;
      simTime += simDt;
      gParts.update(simDt, gMass);

      if (gAutoOrbit && !gDragging) {
        gCam.addYaw(0.25f * dt);
      }

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
      shader.setInt("uLensing", gLensing ? 1 : 0);
      shader.setFloat("uDiskRIn", gDiskRIn);
      shader.setFloat("uDiskROut", 12.0f);
      shader.setFloat("uDiskTilt", gDiskTilt);
      for (std::size_t i = 0; i < kMaxParticles; ++i) {
        char name[32];
        std::snprintf(name, sizeof(name), "uParticles[%zu]", i);
        const glm::vec3& p = gParts.position(i);
        shader.setVec3(name, p.x, p.y, p.z);
      }

      glBindVertexArray(quad);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      const std::string hudText = buildHudText(simTime);
      hud.draw(gFbW, gFbH, hudText.c_str());

      if (gWantShot) {
        gWantShot = false;
        char name[64];
        std::snprintf(name, sizeof(name), "shot_%02d.ppm", shotIdx++);
        savePpm(name);
      }

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quad);

    if (guiTest) {
      const long long d12 = ppmDiff("shot_01.ppm", "shot_02.ppm");
      const long long d23 = ppmDiff("shot_02.ppm", "shot_03.ppm");
      const long long d34 = ppmDiff("shot_03.ppm", "shot_04.ppm");
      std::cout << "ppm diff 1-2 (lensing): " << d12 << "\n";
      std::cout << "ppm diff 2-3 (tilt): " << d23 << "\n";
      std::cout << "ppm diff 3-4 (hud/params): " << d34 << "\n";
      std::cout << "final state: lens=" << (gLensing ? 1 : 0)
                << " pause=" << (gPaused ? 1 : 0)
                << " doppler=" << gDoppler
                << " parts=" << gParts.count()
                << " tilt=" << gDiskTilt << "\n";

      int fails = 0;
      if (d12 < 50000) {
        std::cerr << "FAIL: L prawie nie zmienia obrazu\n";
        ++fails;
      }
      if (d23 < 1000) {
        std::cerr << "FAIL: tilt prawie nie zmienia obrazu\n";
        ++fails;
      }
      if (!gPaused || gDoppler != 0 || gParts.count() != 6 || gDiskTilt <= 0.f) {
        std::cerr << "FAIL: stan po sekwencji klawiszy zly\n";
        ++fails;
      }
      if (fails == 0) {
        std::cout << "selftest gui: PASS\n";
      } else {
        std::cerr << "selftest gui: " << fails << " fails\n";
        exitCode = 1;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "blad: " << e.what() << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return exitCode;
}
