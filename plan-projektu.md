# Projekt: soczewkowanie grawitacyjne (czarna dziura)

Praca semestralna z Grafiki 3D. Aplikacja w OpenGL na Linuxie, grafika w czasie rzeczywistym, interakcja z użytkownikiem, shader GLSL (bazowo ze Shadertoy).

Temat: wizualizacja efektów grawitacyjnych wokół czarnej dziury.

## Co ma robić program

Na ekranie widać czarną dziurę z dyskiem akrecyjnym i zakrzywionym tłem (soczewkowanie). Da się kręcić kamerą myszą, przybliżać/oddalac, zmieniać parametry (masa, czas, wygląd dysku). Opcjonalnie kilka ciał testowych na orbitach.

Z sylabusa: OpenGL w czasie rzeczywistym, interakcja, shadery GLSL, dokumentacja.

## Co biorę (Linux)

Nie mam doświadczenia z bibliotekami okienkowymi z labów, więc biorę coś prostego:

- C++17
- OpenGL 3.3 core (albo 3.3+), shadery GLSL
- GLFW - okno, kontekst, klawiatura, mysz
- GLAD - ładowanie funkcji OpenGL
- glm - macierze / wektory (kamera)
- CMake do budowania

freeglut też by przeszedł (często na labach), ale GLFW wygodniej pod kamerę myszą i nowoczesny kontekst. Jak prowadzący będzie wolał GLUT, da się to później przepiąć.

Zależności systemowe (Ubuntu/Debian mniej więcej):
`libglfw3-dev`, ewentualnie paczki pod OpenGL, kompilator `g++`, `cmake`.

## Jak to technicznie działa

1. Rysujemy jeden pełnoekranowy quad (dwa trójkąty).
2. Vertex shader prawie nic nie robi, tylko podaje współrzędne.
3. Cała magia siedzi we fragment shaderze: raymarching / śledzenie promieni w stronę dziury, dysk, zagięcie światła. Punkt startowy to port shadera ze Shadertoy (trzeba podmienić `iTime`, `iResolution`, `iMouse` na własne uniformy).
4. Program C++ co klatkę wrzuca uniformy (czas, rozdzielczość, pozycja kamery, masa, flagi efektów) i rysuje quada.
5. Bonus: na CPU kilka ciał testowych (Verlet) i wrzucenie pozycji do shadera.

To nie ma być pełna OTW. Model wizualny jak na Shadertoy, w docs trzeba napisać skąd wzory i co upraszczamy.

## Interakcja

- mysz: obrót kamery wokół dziury (orbit)
- scroll albo klawisze: zoom
- klawisze (do ustalenia przy kodzie), np.:
  - `+/-` masa / siła soczewkowania
  - `[` `]` prędkość czasu
  - `1/2/3` warianty wyglądu albo włączenie redshiftu / Dopplera jeśli wejdzie do shadera
  - `R` reset kamery
  - `Esc` wyjście
- opcjonalnie później proste HUD (tekst) albo wypisywanie parametrów w terminalu na start

## Etapy robienia

Kolejność mniej więcej taka, żeby zawsze było co uruchomić:

1. Szkielet CMake + okno GLFW + clear kolorem.
2. Quad + najprostszy fragment shader (np. gradient / czas).
3. Port shadera czarnej dziury ze Shadertoy, podpięcie `uTime`, `uResolution`.
4. Kamera orbit (mysz + zoom), uniformy kamery do shadera.
5. Parametry fizyczne / wizualne na klawiszach.
6. Bonus: orbitujące cząstki.
7. Porządki: README jak zbudować i odpalić, sterowanie, screenshoty.
8. Dokumentacja zaliczeniowa (użytkownika + techniczna).

## Struktura katalogów (plan)

```
Grafika3D/
  plan-projektu.md
  README.md
  CMakeLists.txt
  src/
    main.cpp
    shader.cpp / .h
    camera.cpp / .h
    particles.cpp / .h
    hud.cpp / .h
  shaders/
    fullscreen.vert
    blackhole.frag
  external/stb/
  docs/
```

Nazwy plików mogą się trochę zmienić.

## Skąd shader

Bazowo Shadertoy (trzeba podać konkretne demo w dokumentacji i zachować info o autorze / licencji jeśli jest). Port = przepisanie wejść Shadertoy na własne uniformy i ewentualne uproszczenie, żeby chodziło w rozsądnym FPS na zwykłym laptopie.

Jeśli wybrany shader okaże się za ciężki, bierzemy prostszy wariant albo obniżamy jakość (mniej kroków raymarchingu).

## Ryzyka

- shader za wolny na zintegrowanej grafice -> mniej iteracji, mniejsza rozdzielczość wewnętrzna, prostszy dysk
- port ze Shadertoy bywa upierdliwy (różne konwencje, tekstury) -> zaczynamy od wersji bez zewnętrznych tekstur albo z jedną teksturą proceduralną
- za dużo fizyki na start -> najpierw sam wygląd + kamera, orbity na końcu

## Dokumentacja na zaliczenie

Sylabus chce opis procesu, docs użytkownika i techniczną. W `docs/`: jak odpalić, sterowanie, jak to jest zrobione (okno -> uniformy -> shader), skąd wzory, źródła.

## Stan

Działa program (etapy 1-7 + dopiski): CMake/GLFW/GLAD, quad + shader (tsBXW3), kamera, klawisze, ciała testowe Verlet, HUD (`stb_easy_font`), README, screenshoty, docs.

Dorobione potem: HUD + `H`, Spacja pauza, `A` auto-obrót, `V` gotowe ujęcia, `L` soczewkowanie, MMB / `,` ciała testowe, `U`/`I` rIn, `T`/`Y` tilt, `F` fullscreen, `F12` PPM.

## Co jeszcze dorobić

Raczej nic obowiązkowego. Jak zostanie czas to moze lepsze screeny.

Nie robie drugiej dziury ani Kerra - na zintegrowanej i tak ledwo ciagnie, stąd jakość 1/2/3.
