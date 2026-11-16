#include "hud.h"

#include <glad/glad.h>

#include <stb_easy_font.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace {

const char* kVert = R"(#version 330 core
layout(location = 0) in vec2 aPos;
uniform vec2 uScreen;
uniform float uScale;
void main() {
  // stb: y w dol, OpenGL NDC: y w gore; uScale powieksza font
  float x =  2.0 * (aPos.x * uScale) / uScreen.x - 1.0;
  float y =  1.0 - 2.0 * (aPos.y * uScale) / uScreen.y;
  gl_Position = vec4(x, y, 0.0, 1.0);
}
)";

const char* kFrag = R"(#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
  FragColor = vec4(uColor, 0.95);
}
)";

unsigned int compile(unsigned int type, const char* src) {
  const unsigned int sh = glCreateShader(type);
  glShaderSource(sh, 1, &src, nullptr);
  glCompileShader(sh);
  int ok = 0;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
    glDeleteShader(sh);
    throw std::runtime_error(std::string("hud shader: ") + log);
  }
  return sh;
}

}  // namespace

Hud::Hud() {
  const unsigned int vs = compile(GL_VERTEX_SHADER, kVert);
  const unsigned int fs = compile(GL_FRAGMENT_SHADER, kFrag);
  prog_ = glCreateProgram();
  glAttachShader(prog_, vs);
  glAttachShader(prog_, fs);
  glLinkProgram(prog_);
  glDeleteShader(vs);
  glDeleteShader(fs);

  int ok = 0;
  glGetProgramiv(prog_, GL_LINK_STATUS, &ok);
  if (!ok) {
    throw std::runtime_error("hud: link failed");
  }

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // x,y,z float + color uchar[4] = 16 B; bierzemy tylko xy
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, nullptr);
  glBindVertexArray(0);
}

Hud::~Hud() {
  if (ebo_) glDeleteBuffers(1, &ebo_);
  if (vbo_) glDeleteBuffers(1, &vbo_);
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (prog_) glDeleteProgram(prog_);
}

void Hud::draw(int fbW, int fbH, const char* text) const {
  if (!text || !text[0] || fbW <= 0 || fbH <= 0) {
    return;
  }

  // ~270 B / znak; zapas na dlugi help
  char vbuf[200000];
  unsigned char col[4] = {220, 230, 240, 255};
  const int nq = stb_easy_font_print(8.f, 8.f, const_cast<char*>(text), col, vbuf,
                                     static_cast<int>(sizeof(vbuf)));
  if (nq <= 0) {
    return;
  }

  std::vector<unsigned int> idx(static_cast<std::size_t>(nq) * 6);
  for (int q = 0; q < nq; ++q) {
    const unsigned int b = static_cast<unsigned int>(q * 4);
    const std::size_t o = static_cast<std::size_t>(q * 6);
    idx[o + 0] = b + 0;
    idx[o + 1] = b + 1;
    idx[o + 2] = b + 2;
    idx[o + 3] = b + 0;
    idx[o + 4] = b + 2;
    idx[o + 5] = b + 3;
  }

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, nq * 4 * 16, vbuf, GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(idx.size() * sizeof(unsigned int)), idx.data(),
               GL_STREAM_DRAW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(prog_);
  glUniform2f(glGetUniformLocation(prog_, "uScreen"), static_cast<float>(fbW),
              static_cast<float>(fbH));
  glUniform1f(glGetUniformLocation(prog_, "uScale"), 2.0f);
  glUniform3f(glGetUniformLocation(prog_, "uColor"), 1.0f, 1.0f, 1.0f);
  glDrawElements(GL_TRIANGLES, nq * 6, GL_UNSIGNED_INT, nullptr);

  glDisable(GL_BLEND);
  glBindVertexArray(0);
}
