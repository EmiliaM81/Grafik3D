# Dokumentacja uzytkownika

Program pokazuje czarna dziure z dyskiem i zakrzywionym tlem. Mozna krecic kamera i zmieniac pare parametrow. Linux (u mnie Ubuntu). Aktualne wartosci sa na HUD w oknie; `H` pokazuje skrot klawiszy.

Zaleznosci mniej wiecej: `g++`, `cmake`, OpenGL 3.3+, ewentualnie `libglfw3-dev` `libglm-dev` `libxinerama-dev`. Jak nie ma GLFW/glm, CMake sciagnie przy pierwszym configure (siec). Jest tez `external/x11_extra` jak Xinerama sie wykrzaczy.

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Odpalasz z `build/` - sciezki do shaderow sa wklejone przy kompilacji. W terminalu leci wersja OpenGL; reszta sterowania jest na HUD.

Sterowanie: LMB orbit, scroll zoom. `+/-` masa, `9`/`0` (albo `[` `]`) czas, Spacja pauza, `A` auto-obrot kamery. `Q` jakosc (cykl 1-2-3), `D` Doppler, `L` soczewkowanie (to samo ujecie z/bez). `P` ciala testowe (masy na orbitach), `,` mniej, MMB doklada kolejne (do 24). `U` `I` wewnetrzny promien dysku, `T` `Y` tilt. `V` przelacza gotowe katy (z gory / z boku / ukos). `F` fullscreen, `F12` zrzut do `shot_XX.ppm` w katalogu roboczym (zwykle `build/`). `R` reset, `Esc` wyjscie. Na PL klawiaturze lepiej `9`/`0` niz nawiasy kwadratowe.

Screeny: `docs/screenshots/default.png`, `orbit.png`, `zoom.png`.

Jak cos nie gra: czarne okno to zwykle sterownik (`glxinfo`), niski FPS to wcisnij `1` albo zmniejsz okno, brak GLFW przy cmake to siec albo `apt install libglfw3-dev`.
