# Grafika 3D – czarna dziura

Projekt zaliczeniowy z Grafiki 3D. OpenGL, Linux.  
Na ekranie czarna dziura z dyskiem i soczewkowaniem, kamera mysza, parametry na klawiszach, bonus czastki. Shader GLSL (port ze Shadertoy). Parametry na HUD (`H` = help).

Szczegoly: `plan-projektu.md`, docs w `docs/`.

![widok](docs/screenshots/default.png)

## Stack

C++17, OpenGL 3.3, GLSL, GLFW, GLAD (`external/glad`), glm, stb_easy_font (HUD), CMake.  
GLFW/glm sciaga sie przez FetchContent jak nie ma w systemie.

## Zaleznosci (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev libxinerama-dev
```

Bez paczek GLFW/glm CMake pobierze je przy `cmake ..` (trzeba sieci).  
Jak brakuje Xineramy, jest lokalny `external/x11_extra`. Potrzebny tez sterownik OpenGL.

## Budowanie

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Shadery sa podpinane przez `SHADER_DIR` przy kompilacji, wiec odpalasz z `build/`.

## Sterowanie

- LMB – orbit kamery
- scroll – zoom, Shift+scroll – ogniskowa
- `+/-` – masa / sila soczewki
- `9`/`0` (albo `[` `]`) – predkosc czasu, Spacja – pauza
- `Q` – jakosc (1-2-3), `D` – Doppler, `L` – soczewkowanie on/off
- `P` – zolte kropki on/off, `,` mniej, `G`/`M`/MMB – dokladaj (max 24)
- `U` `I` – rIn dysku, `T` `Y` – tilt
- `V` – gotowe ujecie, `A` – auto-obrot
- `F` – fullscreen, `F12` – screenshot PPM
- `H` – help, `R` – reset, `Esc` – wyjscie

Wiecej screenow: `docs/screenshots/` (orbit, zoom).

## Pliki

`src/` – main, shader, camera, particles, hud  
`shaders/` – fullscreen.vert, blackhole.frag  
`docs/` – uzytkownik, techniczna, zrodla  

Repo: `git@github.com:EmiliaM81/Grafik3D.git`
