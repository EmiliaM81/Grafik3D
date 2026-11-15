# Projekt: soczewkowanie grawitacyjne (czarna dziura)

Praca semestralna z Grafiki 3D. Aplikacja w OpenGL na Linuxie, grafika w czasie rzeczywistym, interakcja z użytkownikiem, shader GLSL (bazowo ze Shadertoy).

Temat roboczy: interaktywna wizualizacja efektów grawitacyjnych wokół czarnej dziury.

## Co ma robić program

Na ekranie widać czarną dziurę z dyskiem akrecyjnym i zakrzywionym tłem (soczewkowanie). Da się kręcić kamerą myszą, przybliżać/oddalac, zmieniać parametry (masa, czas, wygląd dysku). Opcjonalnie kilka cząstek na orbitach liczonych numerycznie.

Wymagania z sylabusa pokryte tak: OpenGL + RT, interakcja, potok programowalny (GLSL), potem dokumentacja i prezentacja.

## Stack (Linux)

Nie mam doświadczenia z bibliotekami okienkowymi z labów, więc biorę coś prostego i popularnego:

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
5. Bonus: na CPU kilka punktów z całowaniem (Verlet albo RK4) pod grawitacją Newtona / orbitą Keplerowską i dorysowanie ich jako punktów/sphere'ów albo jako danych do shadera.

Uwaga: pełna ogólna teoria względności to nie jest cel. Robimy uproszczony, wizualny model (jak w typowych demo Shadertoy), ale w dokumentacji trzeba napisać skąd wzory / jakie uproszczenia.

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
8. Dokumentacja zaliczeniowa (użytkownika + techniczna) i prezentacja.

## Struktura katalogów (plan)

```
Grafika3D/
  plan-projektu.md      <- ten plik
  README.md             <- jak zbudować / sterowanie (później)
  CMakeLists.txt
  src/
    main.cpp
    shader.cpp / .h     <- wczytywanie i kompilacja GLSL
    camera.cpp / .h     <- orbit camera
  shaders/
    fullscreen.vert
    blackhole.frag
  assets/               <- ewentualnie tekstura tła gwiazd
  docs/                 <- dokumentacja na zaliczenie
```

Nazwy plików mogą się lekko zmienić przy pisaniu kodu.

## Skąd shader

Bazowo Shadertoy (trzeba podać konkretne demo w dokumentacji i zachować info o autorze / licencji jeśli jest). Port = przepisanie wejść Shadertoy na własne uniformy i ewentualne uproszczenie, żeby chodziło w rozsądnym FPS na zwykłym laptopie.

Jeśli wybrany shader okaże się za ciężki, bierzemy prostszy wariant albo obniżamy jakość (mniej kroków raymarchingu).

## Ryzyka

- shader za wolny na zintegrowanej grafice -> mniej iteracji, mniejsza rozdzielczość wewnętrzna, prostszy dysk
- port ze Shadertoy bywa upierdliwy (różne konwencje, tekstury) -> zaczynamy od wersji bez zewnętrznych tekstur albo z jedną teksturą proceduralną
- za dużo fizyki na start -> najpierw sam wygląd + kamera, orbity na końcu

## Dokumentacja na zaliczenie (później)

Sylabus chce opis procesu, dokumentację użytkownika i techniczną pod rozbudowę. W `docs/` będzie m.in.: temat i cel, jak odpalić, sterowanie, architektura (okno -> uniformy -> shader), skrót modelu (co liczymy, czego nie), źródła (Shadertoy, Redbook/SuperBible jak coś z książek wejdzie).

## Stan

Na razie jest tylko ten plan. Kod jeszcze nie. Następny krok: szkielet projektu (CMake, GLFW, pusty quad) w tym folderze.
