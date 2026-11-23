# Dokumentacja techniczna

C++17, OpenGL 3.3 core, GLFW, GLAD, glm, stb_easy_font (HUD), CMake (FetchContent na GLFW/glm jak trzeba).

Okno i input przez GLFW. Co klatke: update cial testowych na CPU, uniformy, fullscreen quad, potem overlay tekstu. Vertex quada prawie nic nie robi, obraz liczy fragment shader. HUD to osobny maly program GLSL + quady ze `stb_easy_font`.

Wazniejsze uniformy: `uTime`, `uResolution`, `uCamPos`, `uMass`, `uQuality`, `uDoppler`, `uLensing`, `uDiskRIn` / `uDiskROut` / `uDiskTilt`, oraz `uShowParts` / `uParticles[24]` (ciala testowe).

W shaderze Rs = 1. Kamera skalowana przez `RsWorld = 0.5 * uMass`. Przyspieszenie fotonu (Binet), wylaczalne przez `uLensing`:

`a = -1.5 * h² * x / r⁵`

Verlet, limit krokow z `uQuality`. Dysk w plaszczyznie po obrocie o `uDiskTilt` wokol X. Pod horyzontem czern, jak ucieknie to gwiazdy proceduralne. To nie jest pelna OTW ani Kerr, tylko model wizualny.

Kamera: `OrbitCamera` (yaw/pitch + dystans), presets (`V`), auto-yaw (`A`). Screenshot: `glReadPixels` -> PPM. Fullscreen przez `glfwSetWindowMonitor`.

Ciala testowe: `ParticleSystem` na CPU, Newton + Verlet. `setCount` / `spawn` (MMB). Pozycje do shadera, prosty glow.

Pliki: `src/main.cpp`, `shader.*`, `camera.*`, `particles.*`, `hud.*`, `shaders/fullscreen.vert`, `blackhole.frag`. Zrodla: `docs/zrodla.md`.
