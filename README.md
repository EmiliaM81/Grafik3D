# Grafika 3D – czarna dziura

Projekt zaliczeniowy z Grafiki 3D (OpenGL).  
Wizualizacja soczewkowania wokół czarnej dziury: dysk, zagięte tło, parametry z klawiatury. Shader GLSL, Linux.

Więcej w `plan-projektu.md`.

## Stack

- C++17, OpenGL 3.3 core, GLSL
- GLFW, GLAD (w `external/glad`), glm
- CMake (GLFW/glm ściąga przez FetchContent, jak nie ma w systemie)

## Zależności (Ubuntu/Debian)

Najwygodniej:

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

## Sterowanie (na razie)

- `+` / `-` – masa / siła „soczewki”
- `[` / `]` – prędkość czasu
- `R` – reset parametrów
- `Esc` – wyjście

Kamera orbit (mysz + zoom) jeszcze nie – to następny etap.

## Struktura

```
src/main.cpp, shader.*
shaders/fullscreen.vert, blackhole.frag
external/glad/, external/x11_extra/
```

## Repo

```
git@github.com:EmiliaM81/Grafik3D.git
```

## Status

Działa szkielet: okno GLFW, fullscreen quad, proceduralny shader czarnej dziury + `uTime` / `uResolution` / `uMass`.  
Dalej: kamera orbit, porządniejszy port ze Shadertoy, opcjonalnie cząstki.
