#version 330 core

// uproszczona wizualizacja czarnej dziury (bez tekstur):
// proceduralny dysk + proste zagiecie tla wokól horyzontu.
// pozniej mozna podmienic na porcie z konkretnego Shadertoy.

in vec2 vUv;
out vec4 FragColor;

uniform float uTime;
uniform vec2 uResolution;
uniform float uMass; // 0.5 .. 2.0, skaluje horyzont / sile "soczewki"

float hash(vec2 p) {
  return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// proste gwiazdy proceduralne
float stars(vec2 uv) {
  vec2 g = floor(uv * 45.0);
  float n = hash(g);
  float s = step(0.985, n);
  float tw = 0.6 + 0.4 * sin(uTime * 2.0 + n * 40.0);
  return s * tw;
}

void main() {
  // wspolrzedne wycentrowane, aspect z okna
  vec2 uv = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;

  float mass = clamp(uMass, 0.4, 2.5);
  float rH = 0.11 * mass; // "horyzont"
  float r = length(uv);
  float a = atan(uv.y, uv.x);

  // lekke zagiecie kata / promienia (udawane soczewkowanie)
  float warp = (0.09 * mass) / (r * r + 0.012);
  float bent = a + warp * 0.55;
  float rBent = r - warp * 0.02;

  // tlo gwiazd w zagietych wspolrzednych
  vec2 starUv = vec2(cos(bent), sin(bent)) * max(rBent, 0.001);
  vec3 col = vec3(0.01, 0.015, 0.04);
  col += stars(starUv + vec2(0.0, uTime * 0.01)) * 0.85;
  col += stars(starUv * 1.7 + 3.1) * 0.35;

  // dysk akrecyjny (pierscien)
  float diskInner = rH * 1.55;
  float diskOuter = rH * 4.2;
  float disk = smoothstep(diskInner, diskInner + 0.025, r)
             * smoothstep(diskOuter, diskOuter - 0.08, r);

  // jasniejsze "pasma" + rotacja
  float spin = bent * 3.0 + uTime * (1.4 / max(mass, 0.5)) + 1.2 / max(r, 0.05);
  float bands = pow(0.5 + 0.5 * sin(spin), 2.5);
  float heat = disk * (0.35 + 0.65 * bands);

  vec3 diskCol = mix(vec3(0.7, 0.15, 0.02), vec3(1.4, 0.85, 0.35), bands);
  col += diskCol * heat;

  // cienki pierscien przy horyzoncie
  float ring = exp(-abs(r - rH * 1.15) * (70.0 / mass));
  col += ring * vec3(0.55, 0.7, 1.0) * 0.55 * (1.0 - smoothstep(rH, rH * 0.85, r));

  // event horizon
  float horizon = smoothstep(rH, rH * 0.82, r);
  col *= horizon;

  // delikatny vignette
  col *= 1.0 - 0.25 * length(uv);

  FragColor = vec4(col, 1.0);
}
