#pragma once

class Hud {
public:
  Hud();
  ~Hud();

  Hud(const Hud&) = delete;
  Hud& operator=(const Hud&) = delete;

  void draw(int fbW, int fbH, const char* text) const;

private:
  unsigned int prog_ = 0;
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
};
