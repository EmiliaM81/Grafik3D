#pragma once

#include <glm/glm.hpp>

class OrbitCamera {
public:
  OrbitCamera();

  void reset();

  void setView(float yaw, float pitch, float distance);

  void orbit(float dx, float dy);

  void addYaw(float dyaw);

  void zoom(float scrollY);

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
