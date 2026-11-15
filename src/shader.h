#pragma once

#include <string>

// program GLSL: wczytanie z plikow, kompilacja, link

class Shader {
public:
  // wczytuje vert+frag, kompiluje; rzuca runtime_error jak cos nie gra
  Shader(const std::string& vertPath, const std::string& fragPath);
  ~Shader();

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  void use() const;

  void setFloat(const char* name, float v) const;
  void setVec2(const char* name, float x, float y) const;
  void setVec3(const char* name, float x, float y, float z) const;
  void setInt(const char* name, int v) const;

  unsigned int id() const { return program_; }

private:
  unsigned int program_ = 0;

  static std::string readFile(const std::string& path);
  static unsigned int compile(unsigned int type, const std::string& src);
};
