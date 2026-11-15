# Dokumentacja techniczna

C++17, OpenGL 3.3 core, GLFW, GLAD, glm, CMake (FetchContent na GLFW/glm jak trzeba).

Okno i input przez GLFW. Co klatke: update czastek na CPU, ustawienie uniformow, rysowanie jednego fullscreen quada (2 trojkaty). Vertex prawie nic nie robi – caly obraz liczy fragment shader (ray marching / geodezyjna fotonu).

Wazniejsze uniformy: `uTime`, `uResolution` (jak iTime/iResolution), `uCamPos`, `uMass`, `uQuality`, `uDoppler`, oraz `uShowParts` / `uParticles[8]` pod bonus.

W shaderze Rs = 1. Kamera skalowana przez `RsWorld = 0.5 * uMass`. Przyspieszenie fotonu (Binet):

`a = -1.5 * h² * x / r⁵`

Verlet, limit krokow z `uQuality`. Dysk w `y=0` – przod zaslania dziure, tyl wychodzi lukiem. Pod horyzontem czern, jak ucieknie to gwiazdy proceduralne + photon ring ~1.5 Rs. To nie jest pelna OTW ani Kerr, tylko model wizualny.

Kamera: `OrbitCamera` (yaw/pitch + dystans), pozycja do `uCamPos`.

Czastki: `ParticleSystem` na CPU, Newton + Verlet, start prawie keplerowski. Pozycje do shadera, prosty glow. Wlacznik `P`.

Pliki: `src/main.cpp`, `shader.*`, `camera.*`, `particles.*`, `shaders/fullscreen.vert`, `shaders/blackhole.frag`. Zrodla: `docs/zrodla.md`.
