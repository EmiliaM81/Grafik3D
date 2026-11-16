#version 330 core

// Rs=1, a=-1.5 h^2 x/r^5, Verlet.
// dysk lekko przechylony (uDiskTilt), rIn/rOut z CPU.
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
uniform vec3 uParticles[24];
uniform float uFocal;
uniform int uLensing;
uniform float uDiskRIn;
uniform float uDiskROut;
uniform float uDiskTilt;

float hash(vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// zwykle tlo nieba (stale)
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

vec3 toDisk(vec3 p) {
  float ct = cos(uDiskTilt);
  float st = sin(uDiskTilt);
  return vec3(p.x, p.y * ct - p.z * st, p.y * st + p.z * ct);
}

void main() {
  vec2 p = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;

  float mass = clamp(uMass, 0.4, 2.5);
  float RsWorld = 0.5 * mass;
  vec3 ro = uCamPos / RsWorld;

  const float rH = 1.0;
  float rIn = clamp(uDiskRIn, 1.6, 8.0);
  float rOut = clamp(uDiskROut, rIn + 2.0, 18.0);

  vec3 ww = normalize(-ro);
  vec3 up = abs(ww.y) > 0.9 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
  vec3 uu = normalize(cross(ww, up));
  vec3 vv = cross(uu, ww);
  float focal = clamp(uFocal, 0.8, 5.0);
  vec3 rd = normalize(p.x * uu + p.y * vv + focal * ww);

  float h2 = dot(cross(ro, rd), cross(ro, rd));

  vec3 col = vec3(0.01, 0.012, 0.03);
  float transm = 1.0;
  bool captured = false;

  int q = clamp(uQuality, 1, 3);
  int maxSteps = 220;
  float stepMul = 1.35;
  if (q == 2) { maxSteps = 450; stepMul = 1.0; }
  if (q >= 3) { maxSteps = 700; stepMul = 0.72; }

  vec3 pos = ro;
  vec3 vel = rd;
  float yPrev = toDisk(pos).y;
  int hits = 0;

  for (int i = 0; i < 700; ++i) {
    if (i >= maxSteps) break;

    float r2 = dot(pos, pos);
    float r = sqrt(r2);

    if (r < rH) {
      captured = true;
      break;
    }
    if (r > 70.0 || transm < 0.015) break;

    float dt = clamp(0.045 * r * stepMul, 0.015 * stepMul, 0.14 * stepMul);
    vec3 pos0 = pos;

    vec3 accel = vec3(0.0);
    if (uLensing != 0) {
      accel = (-1.5 * h2) * pos / (r2 * r2 * r);
    }
    pos += vel * dt + 0.5 * accel * dt * dt;

    float r2n = dot(pos, pos);
    float rn = sqrt(r2n);
    if (rn < rH) {
      captured = true;
      break;
    }
    vec3 accel2 = vec3(0.0);
    if (uLensing != 0) {
      accel2 = (-1.5 * h2) * pos / (r2n * r2n * rn);
    }
    vel += 0.5 * (accel + accel2) * dt;

    float y = toDisk(pos).y;
    if (yPrev * y <= 0.0 && transm > 0.02 && hits < 6) {
      float f = abs(yPrev) / (abs(yPrev) + abs(y) + 1e-6);
      vec3 hit = toDisk(mix(pos0, pos, f));
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

    // zolte kropki = orbitujace czastki
    if (uShowParts != 0 && uPartCount > 0) {
      for (int k = 0; k < 24; ++k) {
        if (k >= uPartCount) break;
        vec3 pp = uParticles[k] / RsWorld;
        float d = length(pos - pp);
        float rad = 0.28;
        if (d < rad) {
          float g = smoothstep(rad, 0.0, d);
          g = g * g;
          col += transm * vec3(2.0, 1.55, 0.55) * g * 1.35;
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
