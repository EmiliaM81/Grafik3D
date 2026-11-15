# Dokumentacja uzytkownika

Program pokazuje czarna dziure z dyskiem i zakrzywionym tlem. Mozna krecic kamera i zmieniac pare parametrow. Linux (u mnie Ubuntu).

Zaleznosci mniej wiecej: `g++`, `cmake`, OpenGL 3.3+, ewentualnie `libglfw3-dev` `libglm-dev` `libxinerama-dev`. Jak nie ma GLFW/glm, CMake sciagnie przy pierwszym configure (siec). Jest tez `external/x11_extra` jak Xinerama sie wykrzaczy.

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Odpalasz z `build/` – sciezki do shaderow sa wklejone przy kompilacji. W terminalu leci wersja OpenGL i skrot klawiszy.

Sterowanie: LMB orbit, scroll zoom, `+/-` masa, `[` `]` czas, `1/2/3` jakosc, `D` Doppler, `P` czastki, `R` reset, `Esc` wyjscie. Zmiany parametrow wypisuja sie w terminalu.

Screeny: `docs/screenshots/default.png`, `orbit.png`, `zoom.png`.

Jak cos nie gra: czarne okno to zwykle sterownik (`glxinfo`), niski FPS to wcisnij `1` albo zmniejsz okno, brak GLFW przy cmake to siec albo `apt install libglfw3-dev`.
