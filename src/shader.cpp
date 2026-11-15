#include "shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
  const std::string vertSrc = readFile(vertPath);
  const std::string fragSrc = readFile(fragPath);

  const unsigned int vs = compile(GL_VERTEX_SHADER, vertSrc);
  const unsigned int fs = compile(GL_FRAGMENT_SHADER, fragSrc);

  program_ = glCreateProgram();
  glAttachShader(program_, vs);
  glAttachShader(program_, fs);
  glLinkProgram(program_);

  int ok = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
    glDeleteShader(vs);
    glDeleteShader(fs);
    throw std::runtime_error(std::string("link shaderow: ") + log);
  }

  glDeleteShader(vs);
  glDeleteShader(fs);
}

Shader::~Shader() {
  if (program_ != 0) {
    glDeleteProgram(program_);
  }
}

void Shader::use() const {
  glUseProgram(program_);
}

void Shader::setFloat(const char* name, float v) const {
  glUniform1f(glGetUniformLocation(program_, name), v);
}

void Shader::setVec2(const char* name, float x, float y) const {
  glUniform2f(glGetUniformLocation(program_, name), x, y);
}

void Shader::setVec3(const char* name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(program_, name), x, y, z);
}

void Shader::setInt(const char* name, int v) const {
  glUniform1i(glGetUniformLocation(program_, name), v);
}

std::string Shader::readFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("nie moge otworzyc: " + path);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

unsigned int Shader::compile(unsigned int type, const std::string& src) {
  const unsigned int sh = glCreateShader(type);
  const char* csrc = src.c_str();
  glShaderSource(sh, 1, &csrc, nullptr);
  glCompileShader(sh);

  int ok = 0;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[1024];
    glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
    const char* kind = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
    glDeleteShader(sh);
    throw std::runtime_error(std::string("kompilacja ") + kind + ": " + log);
  }
  return sh;
}
