#include "particles.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

glm::vec3 ParticleSystem::accel(const glm::vec3& p, float gm) {
  const float r2 = glm::dot(p, p) + 0.08f;
  const float inv = gm / (r2 * std::sqrt(r2));
  return -p * inv;
}

void ParticleSystem::initOne(std::size_t i, float gm) {
  const float t = static_cast<float>(i) / static_cast<float>(kMaxParticles);
  const float ang = t * static_cast<float>(M_PI) * 2.f + 0.35f * static_cast<float>(i);
  const float r = 4.2f + t * 5.5f;
  const float y = (i % 2 == 0) ? 0.15f : -0.12f;
  pos_[i] = glm::vec3(std::cos(ang) * r, y, std::sin(ang) * r);

  const float speed = std::sqrt(std::max(gm, 0.05f) / r);
  vel_[i] = glm::vec3(-std::sin(ang) * speed, 0.f, std::cos(ang) * speed);
}

ParticleSystem::ParticleSystem() {
  reset(1.0f);
}

void ParticleSystem::reset(float gm) {
  for (std::size_t i = 0; i < kMaxParticles; ++i) {
    initOne(i, gm);
  }
  count_ = 16;
}

void ParticleSystem::setCount(std::size_t n, float gm) {
  n = std::min(n, kMaxParticles);
  if (n > count_) {
    for (std::size_t i = count_; i < n; ++i) {
      initOne(i, gm);
    }
  }
  count_ = n;
}

void ParticleSystem::spawn(float gm) {
  if (count_ >= kMaxParticles) {
    return;
  }
  initOne(count_, gm);
  const float ang = static_cast<float>(count_) * 1.7f;
  const float r = std::min(12.0f, 4.5f + static_cast<float>(count_ % 12) * 0.55f);
  pos_[count_] = glm::vec3(std::cos(ang) * r, 0.08f, std::sin(ang) * r);
  const float speed = std::sqrt(std::max(gm, 0.05f) / r);
  vel_[count_] = glm::vec3(-std::sin(ang) * speed, 0.f, std::cos(ang) * speed);
  ++count_;
}

void ParticleSystem::update(float dt, float gm) {
  const int sub = std::max(1, static_cast<int>(dt / 0.016f));
  const float h = dt / static_cast<float>(sub);

  for (int s = 0; s < sub; ++s) {
    for (std::size_t i = 0; i < count_; ++i) {
      const glm::vec3 a0 = accel(pos_[i], gm);
      pos_[i] += vel_[i] * h + 0.5f * a0 * h * h;
      const glm::vec3 a1 = accel(pos_[i], gm);
      vel_[i] += 0.5f * (a0 + a1) * h;

      const float r = glm::length(pos_[i]);
      if (r < 1.2f || r > 18.f) {
        const float ang = static_cast<float>(i) * 0.9f;
        const float rr = 5.0f + static_cast<float>(i) * 0.7f;
        pos_[i] = glm::vec3(std::cos(ang) * rr, 0.1f, std::sin(ang) * rr);
        const float speed = std::sqrt(std::max(gm, 0.05f) / rr);
        vel_[i] = glm::vec3(-std::sin(ang) * speed, 0.f, std::cos(ang) * speed);
      }
    }
  }
}
