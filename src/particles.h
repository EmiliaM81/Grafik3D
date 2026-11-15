#pragma once

#include <glm/glm.hpp>

#include <array>
#include <cstddef>

// kilka cial na orbitach (Verlet + Newton), potem jako uniformy do shadera

inline constexpr std::size_t kMaxParticles = 8;

class ParticleSystem {
public:
  ParticleSystem();

  // GM w jednostkach sceny (Rs ~ 0.5 * mass w shaderze)
  void reset(float gm);

  // dt w sekundach symulacji (juz po timeScale)
  void update(float dt, float gm);

  std::size_t count() const { return count_; }
  const glm::vec3& position(std::size_t i) const { return pos_[i]; }

private:
  std::size_t count_ = kMaxParticles;
  std::array<glm::vec3, kMaxParticles> pos_{};
  std::array<glm::vec3, kMaxParticles> vel_{};

  static glm::vec3 accel(const glm::vec3& p, float gm);
};
