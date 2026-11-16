#pragma once

// prosty tekst na ekranie (stb_easy_font -> trojkaty)

class Hud {
public:
  Hud();
  ~Hud();

  Hud(const Hud&) = delete;
  Hud& operator=(const Hud&) = delete;

  // rysuje multiline ASCII w lewym gornym rogu (y w dol)
  void draw(int fbW, int fbH, const char* text) const;

private:
  unsigned int prog_ = 0;
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int ebo_ = 0;
};
