#version 330 core

// Rs=1, a=-1.5 h^2 x/r^5, Verlet.
// dysk w y=0: przod zaslania dziure, tyl wychodzi lukiem nad/pod.
// Shadertoy tsBXW3 / Binet, uniformy: uTime, uResolution, uCamPos.

in vec2 vUv;
out vec4 FragColor;

uniform float uTime;
uniform vec2 uResolution;
uniform float uMass;
uniform vec3 uCamPos;
uniform int uQuality;
uniform int uDoppler;
uniform int uShowParts;
uniform int uPartCount;
uniform vec3 uParticles[8];

float hash(vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float stars(vec3 d) {
  d = normalize(d);
  float s = 0.0;
  for (int i = 0; i < 3; ++i) {
    float sc = 44.0 + float(i) * 58.0;
    vec3 c = floor(d * sc);
    float n = hash(c.xy + c.z * 5.0 + float(i) * 17.0);
    if (n > 0.983) {
      s += smoothstep(0.035, 0.0, length(fract(d * sc) - 0.5))
         * (0.5 + 0.5 * sin(uTime * 2.0 + n * 40.0));
    }
  }
  return s;
}

vec3 diskCol(vec2 xz) {
  float rho = length(xz);
  float a = atan(xz.y, xz.x);
  float w = a * 2.0 + uTime * 1.2 + 1.5 / max(rho, 0.4);
  float bands = 0.55 + 0.45 * pow(0.5 + 0.5 * sin(w), 1.5);
  float t = smoothstep(3.0, 11.0, rho);
  vec3 c = mix(vec3(2.2, 1.7, 1.0), vec3(1.3, 0.35, 0.05), t);
  c *= bands;
  if (uDoppler != 0) {
    float dop = 0.55 + 0.6 * clamp(xz.x / max(rho, 0.01), -1.0, 1.0);
    c *= max(dop, 0.35);
  }
  return c * (3.0 / (0.35 + rho));
}

void main() {
  vec2 p = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;

  float mass = clamp(uMass, 0.4, 2.5);
  float RsWorld = 0.5 * mass;
  vec3 ro = uCamPos / RsWorld;

  const float rH = 1.0;
  const float rIn = 2.8;
  const float rOut = 12.0;

  vec3 ww = normalize(-ro);
  vec3 up = abs(ww.y) > 0.9 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
  vec3 uu = normalize(cross(ww, up));
  vec3 vv = cross(uu, ww);
  vec3 rd = normalize(p.x * uu + p.y * vv + 2.0 * ww);

  float h2 = dot(cross(ro, rd), cross(ro, rd));

  vec3 col = vec3(0.01, 0.012, 0.03);
  float transm = 1.0;
  bool captured = false;

  int maxSteps = 300;
  if (uQuality >= 2) maxSteps = 450;
  if (uQuality >= 3) maxSteps = 600;

  vec3 pos = ro;
  vec3 vel = rd;
  float yPrev = pos.y;
  int hits = 0;

  for (int i = 0; i < 600; ++i) {
    if (i >= maxSteps) break;

    float r2 = dot(pos, pos);
    float r = sqrt(r2);

    if (r < rH) {
      captured = true;
      break;
    }
    if (r > 70.0 || transm < 0.015) break;

    float dt = clamp(0.045 * r, 0.02, 0.12);
    vec3 pos0 = pos;

    vec3 accel = (-1.5 * h2) * pos / (r2 * r2 * r);
    pos += vel * dt + 0.5 * accel * dt * dt;

    float r2n = dot(pos, pos);
    float rn = sqrt(r2n);
    if (rn < rH) {
      captured = true;
      break;
    }
    vec3 accel2 = (-1.5 * h2) * pos / (r2n * r2n * rn);
    vel += 0.5 * (accel + accel2) * dt;

    float y = pos.y;
    // tylko przy przecieciu plaszczyzny
    if (yPrev * y <= 0.0 && transm > 0.02 && hits < 6) {
      float f = abs(yPrev) / (abs(yPrev) + abs(y) + 1e-6);
      vec3 hit = mix(pos0, pos, f);
      hit.y = 0.0;

      float rho = length(hit.xz);
      if (rho > rIn && rho < rOut) {
        float m = smoothstep(rIn, rIn + 0.5, rho)
                * smoothstep(rOut, rOut - 1.2, rho);
        vec3 emit = diskCol(hit.xz) * m;
        float dens = (hits == 0) ? 0.85 * m : 0.7 * m;
        col += transm * emit * dens;
        transm *= (1.0 - dens * 0.4);
        hits++;
      }
    }
    yPrev = y;

    if (uShowParts != 0 && uPartCount > 0) {
      for (int k = 0; k < 8; ++k) {
        if (k >= uPartCount) break;
        vec3 pp = uParticles[k] / RsWorld;
        float d = length(pos - pp);
        if (d < 0.18) {
          float g = smoothstep(0.18, 0.0, d);
          col += transm * vec3(1.3, 1.05, 0.5) * g * 0.9;
        }
      }
    }
  }

  if (!captured) {
    col += transm * (stars(vel) + vec3(0.012, 0.016, 0.04));
  }

  col = 1.0 - exp(-col);
  FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
