#pragma once

#include <glm/glm.hpp>

// kamera orbit: kreci sie wokol srodka (dziury), zoom = dystans

class OrbitCamera {
public:
  OrbitCamera();

  // yaw/pitch w radianach, dystans od srodka
  void reset();

  // gotowe ujecie (yaw, pitch, dystans)
  void setView(float yaw, float pitch, float distance);

  // przesuniecie myszy przy trzymanym LMB (piksele)
  void orbit(float dx, float dy);

  // auto-obrot wokol osi Y (radiany)
  void addYaw(float dyaw);

  // scroll: dodatni = przybliz
  void zoom(float scrollY);

  // pozycja oka w world space
  glm::vec3 position() const;

  float yaw() const { return yaw_; }
  float pitch() const { return pitch_; }
  float distance() const { return distance_; }

private:
  float yaw_;
  float pitch_;
  float distance_;

  static constexpr float kMinDist = 4.f;
  static constexpr float kMaxDist = 22.f;
  static constexpr float kDefaultDist = 10.f;
};
