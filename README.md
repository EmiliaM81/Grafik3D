# Grafika 3D – czarna dziura

Projekt zaliczeniowy z przedmiotu Grafika 3D (OpenGL).  
Interaktywna wizualizacja soczewkowania grawitacyjnego wokół czarnej dziury: dysk akrecyjny, zagięcie tła, sterowanie kamerą i parametrami. Shader GLSL (port ze Shadertoy), aplikacja na Linuxie.

Szczegóły planu: `plan-projektu.md`.

## Stack

- C++17, OpenGL 3.3+, GLSL
- GLFW, GLAD, glm
- CMake

## Zależności (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake libglfw3-dev
```

Potrzebny też działający sterownik OpenGL (mesa albo vendor).

## Budowanie i uruchomienie

Kod aplikacji jeszcze nie jest w repo (na razie plan). Jak szkielet wejdzie, będzie mniej więcej tak:

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole   # nazwa binarki do potwierdzenia przy CMake
```

Odpalaj z katalogu, z którego widać `shaders/` (albo ustawimy ścieżki w kodzie).

## Sterowanie (planowane)

- mysz – obrót kamery
- scroll / klawisze – zoom
- `+` / `-` – masa / siła soczewkowania
- `[` / `]` – prędkość czasu
- `R` – reset kamery
- `Esc` – wyjście

Dokładne bindingi dopiszemy jak będzie działać input.

## Repo

```
git@github.com:EmiliaM81/Grafik3D.git
```

## Status

Jest plan projektu. Następny krok: szkielet CMake + okno GLFW + fullscreen quad, potem port shadera czarnej dziury.
