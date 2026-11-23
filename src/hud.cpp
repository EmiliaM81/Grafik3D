#include "hud.h"

#include <glad/glad.h>

#include <stb_easy_font.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

const char* kVert = R"(#version 330 core
layout(location = 0) in vec2 aPos;
void main() {
  gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* kFrag = R"(#version 330 core
out vec4 FragColor;
void main() {
  FragColor = vec4(1.0, 1.0, 1.0, 0.95);
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

void pushNdc(std::vector<float>& out, float px, float py, float fbW, float fbH,
             float scale) {
  out.push_back(2.f * (px * scale) / fbW - 1.f);
  out.push_back(1.f - 2.f * (py * scale) / fbH);
}

}

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
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
  glBindVertexArray(0);
}

Hud::~Hud() {
  if (vbo_) glDeleteBuffers(1, &vbo_);
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (prog_) glDeleteProgram(prog_);
}

void Hud::draw(int fbW, int fbH, const char* text) const {
  if (!text || !text[0] || fbW <= 0 || fbH <= 0) {
    return;
  }

  char vbuf[200000];
  unsigned char col[4] = {255, 255, 255, 255};
  const int nq = stb_easy_font_print(8.f, 8.f, const_cast<char*>(text), col, vbuf,
                                     static_cast<int>(sizeof(vbuf)));
  if (nq <= 0) {
    return;
  }

  constexpr float kScale = 2.f;
  const float w = static_cast<float>(fbW);
  const float h = static_cast<float>(fbH);

  struct StbV {
    float x, y, z;
    std::uint8_t rgba[4];
  };
  const StbV* verts = reinterpret_cast<const StbV*>(vbuf);

  std::vector<float> tris;
  tris.reserve(static_cast<std::size_t>(nq) * 12);
  for (int q = 0; q < nq; ++q) {
    const StbV& a = verts[q * 4 + 0];
    const StbV& b = verts[q * 4 + 1];
    const StbV& c = verts[q * 4 + 2];
    const StbV& d = verts[q * 4 + 3];

    pushNdc(tris, a.x, a.y, w, h, kScale);
    pushNdc(tris, b.x, b.y, w, h, kScale);
    pushNdc(tris, c.x, c.y, w, h, kScale);

    pushNdc(tris, a.x, a.y, w, h, kScale);
    pushNdc(tris, c.x, c.y, w, h, kScale);
    pushNdc(tris, d.x, d.y, w, h, kScale);
  }

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(tris.size() * sizeof(float)), tris.data(),
               GL_STREAM_DRAW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(prog_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(tris.size() / 2));

  glDisable(GL_BLEND);
  glBindVertexArray(0);
}
