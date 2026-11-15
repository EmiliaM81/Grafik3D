# Dokumentacja techniczna (skrót)

Aplikacja C++17 + OpenGL 3.3 core. Okno i input przez GLFW, funkcje GL przez GLAD, wektory przez glm. Budowanie: CMake.

## Przepływ klatki

1. GLFW poll + update cząstek na CPU (Verlet).
2. Clear, ustawienie uniformów w programie GLSL.
3. Rysowanie fullscreen quada (2 trójkąty). Cały obraz liczy fragment shader.

Vertex shader prawie nic nie robi. Cała scena to ray marching / całkowanie geodezyjnej fotonu we fragmencie.

## Uniformy (ważniejsze)

- `uTime`, `uResolution` – jak iTime / iResolution ze Shadertoy
- `uCamPos` – pozycja kamery orbit
- `uMass` – skala Rs / siła zagięcia
- `uQuality` – limit kroków
- `uDoppler`, `uShowParts`, `uParticles[]` – efekty i bonus

## Model w shaderze

Jednostki: Rs = 1 w przestrzeni całkowania. Promień startuje z kamery, przyspieszenie fotonu w postaci Bineta:

`a = -1.5 * h^2 * x / r^5`

(h^2 z momentu kątowego). Integracja velocity Verlet. Przy przecięciu płaszczyzny y=0 składany jest dysk (kilka obrazów – „łuk” z tyłu). Promień wpadający pod horyzont = czerń. Jak ucieknie – gwiazdy proceduralne + cienki photon ring ~1.5 Rs.

To nie jest pełna OTW ani Kerr. Cel: wygląd jak w typowych demo Shadertoy, z opisem uproszczeń.

## Cząstki (bonus)

`ParticleSystem` na CPU: Newton `a = -GM r / r^3` + Verlet, start prawie-keplerowski. Pozycje idą do shadera jako `uParticles[8]` i są „doklejane” przy marszu promienia (prosty glow).

## Pliki

- `src/main.cpp` – pętla, input, uniformy
- `src/shader.*` – load/compile GLSL
- `src/camera.*` – orbit
- `src/particles.*` – Verlet
- `shaders/fullscreen.vert`, `shaders/blackhole.frag`

Źródła inspiracji: `docs/zrodla.md`.
