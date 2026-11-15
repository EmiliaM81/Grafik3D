# Grafika 3D – czarna dziura

Projekt zaliczeniowy z Grafiki 3D (OpenGL).  
Wizualizacja soczewkowania wokół czarnej dziury: dysk, zagięte tło, kamera orbit, parametry z klawiatury, bonus cząstki. Shader GLSL (styl Shadertoy), Linux.

Więcej w `plan-projektu.md`. Docs zaliczeniowe: `docs/`.

## Stack

- C++17, OpenGL 3.3 core, GLSL
- GLFW, GLAD (w `external/glad`), glm
- CMake (GLFW/glm ściąga przez FetchContent, jak nie ma w systemie)

## Zależności (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev libxinerama-dev
```

Jak nie chcesz pakietów GLFW/glm – CMake je pobierze przy pierwszym `cmake ..` (trzeba sieci).  
Przy samym FetchContent nadal przydają się nagłówki X11 (u nas jest lokalny `external/x11_extra` pod Xinerama).

Potrzebny też sterownik OpenGL (mesa / vendor).

## Budowanie i uruchomienie

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Ścieżki do `shaders/` są wklejane przy kompilacji (`SHADER_DIR`), więc możesz odpalać z `build/`.

## Sterowanie

- LMB + przeciąganie – obrót kamery wokół dziury
- scroll – zoom
- `+` / `-` – masa / siła „soczewki”
- `[` / `]` – prędkość czasu
- `1` / `2` / `3` – jakość (kroki w shaderze)
- `D` – Doppler na dysku
- `P` – orbitujące cząstki
- `R` – reset kamery i parametrów
- `Esc` – wyjście

## Struktura

```
src/main.cpp, shader.*, camera.*, particles.*
shaders/fullscreen.vert, blackhole.frag
docs/uzytkownik.md, techniczna.md, zrodla.md
external/glad/, external/x11_extra/
```

## Repo

```
git@github.com:EmiliaM81/Grafik3D.git
```

## Status

Działa shader w stylu Shadertoy (port + atrybucja w `docs/zrodla.md`), kamera, klawisze, cząstki Verlet.  
Dalej: screenshoty / prezentacja, dopieszczanie docs pod oddanie.
