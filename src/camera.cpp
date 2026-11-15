#include "camera.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

OrbitCamera::OrbitCamera() {
  reset();
}

void OrbitCamera::reset() {
  // prawie edge-on: jasny pas dysku przez srodek + luk z tylu nad dziura
  yaw_ = 0.2f;
  pitch_ = 0.22f;
  distance_ = 11.0f;
}

void OrbitCamera::orbit(float dx, float dy) {
  const float sens = 0.005f;
  yaw_ += dx * sens;
  pitch_ -= dy * sens;

  const float lim = static_cast<float>(M_PI) * 0.48f;
  pitch_ = std::clamp(pitch_, -lim, lim);
}

void OrbitCamera::zoom(float scrollY) {
  distance_ *= (scrollY > 0.f) ? 0.9f : 1.1f;
  distance_ = std::clamp(distance_, kMinDist, kMaxDist);
}

glm::vec3 OrbitCamera::position() const {
  const float cp = std::cos(pitch_);
  const float sp = std::sin(pitch_);
  const float cy = std::cos(yaw_);
  const float sy = std::sin(yaw_);
  return glm::vec3(
      distance_ * cp * sy,
      distance_ * sp,
      distance_ * cp * cy);
}
