#pragma once

#include <glm/glm.hpp>

#include <array>
#include <cstddef>

inline constexpr std::size_t kMaxParticles = 24;

class ParticleSystem {
public:
  ParticleSystem();

  void reset(float gm);

  void setCount(std::size_t n, float gm);

  void spawn(float gm);

  void update(float dt, float gm);

  std::size_t count() const { return count_; }
  const glm::vec3& position(std::size_t i) const { return pos_[i]; }

private:
  std::size_t count_ = kMaxParticles;
  std::array<glm::vec3, kMaxParticles> pos_{};
  std::array<glm::vec3, kMaxParticles> vel_{};

  static glm::vec3 accel(const glm::vec3& p, float gm);
  void initOne(std::size_t i, float gm);
};
