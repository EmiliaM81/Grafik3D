# Czarna dziura - soczewkowanie grawitacyjne

Projekt zaliczeniowy z Grafiki 3D. OpenGL na Linuxie.
Na ekranie widać czarną dziurę, jasny dysk wokół niej i gwiazdy w tle, które się zakrzywiają
(to właśnie "soczewkowanie" - światło zgina się koło masy).
Kamerę kręcę myszą, parametry zmieniam klawiszami. W rogu okna jest tekst z aktualnymi
wartościami (HUD = nakładka z info, jak w grze). Bonus: małe ciała testowe krążące po orbitach.

![widok](docs/screenshots/default.png)

## O czym to jest

Pokazuję efekty grawitacji wokół czarnej dziury. To nie jest prawdziwa ogólna teoria względności
z uczelni - tylko model wizualny, żeby wyglądało wiarygodnie i chodziło płynnie na laptopie.


## Jak to powstawało

Robiłam etapami, żeby zawsze było co odpalić.

1. Najpierw samo okno i budowanie projektu.
   CMake składa program. GLFW to biblioteka od okna, myszy i klawiatury.
   Na start okno tylko czyściło się na jeden kolor - żeby sprawdzić, że OpenGL w ogóle działa.

2. Zamiast budować scenę z wielu obiektów, rysuję jeden prostokąt na całe okno
   (w OpenGL to dwa trójkąty sklejone w prostokąt). Na każdym pikselu tego prostokąta
   odpala się fragment shader - program na GPU, który dla każdego piksela liczy kolor.
   Na tym etapie był to jeszcze prosty test (np. gradient), żeby sprawdzić że pipeline działa.

3. Potem wrzuciłam właściwy shader czarnej dziury.
   Bazowałam na demie ze strony Shadertoy (gotowe shadery w przeglądarce), konkretnie `tsBXW3`.
   Nie da się tego wkleić 1:1 - Shadertoy ma swoje zmienne (`iTime`, `iResolution` itd.),
   a u mnie okno jest z GLFW, więc podmieniłam to na własne parametry (uniformy),
   które C++ wysyła co klatkę do GPU. Kamerę też liczę sama w C++, nie jak na Shadertoy.

4. Kamera: lewy przycisk myszy obraca widok wokół dziury, scroll przybliża/oddal a.
   Pozycja kamery idzie do shadera jako jedna zmienna `uCamPos`.

5. Klawisze do parametrów: masa (siła zagięcia), prędkość czasu, jakość obrazu,
   efekt Dopplera na dysku, kształt dysku. Żeby na żywo pokazać "co się dzieje jak zmienię X".

6. Bonus: ciała testowe.
   To nie modele 3D. Pozycje liczę na procesorze (prosta grawitacja Newtona + krok Verlet,
   czyli numeryczne całkowanie ruchu). Co klatkę wysyłam listę pozycji do shadera
   (max 24), a shader tylko dorysowuje przy nich jasną plamkę gdy promień przechodzi obok.

7. Na koniec napisy na ekranie (opis niżej), help na `H`, pauza, auto-obrót,
   gotowe ujęcia kamery, fullscreen, zrzut ekranu do pliku PPM, plus docs i README.

Na słabszej grafice (zintegrowanej) shader bywa wolny. Dlatego jest jakość `Q` (1/2/3).


## Jak są zrobione napisy (HUD)

Nie używam czcionek systemowych ani FreeType. Nie ma też tekstury z literami.

Tekst składam w C++ (`buildHudText` w `main.cpp`) - zwykły string z masą, czasem, flagami itd.
Potem `Hud::draw` bierze ten string i korzysta z `stb_easy_font` (jeden plik nagłówkowy w `external/stb/`).

`stb_easy_font_print` zamienia litery na listę małych prostokątów (każda kreska litery = kwadracik
z czterema wierzchołkami). Ja te kwadraty dzielę na trójkąty, przeliczam współrzędne z pikseli
na układ OpenGL (-1..1) i rysuję je osobnym, bardzo prostym programem GLSL:
vertex tylko ustawia pozycję, fragment maluje prawie białym kolorem. Włączony jest blend,
żeby napisy ładnie leżały na obrazie czarnej dziury.

To osobna warstwa: najpierw cały ekran z dziurą, potem na wierzchu te trójkąty z tekstem.
Kod jest w `src/hud.cpp`. Po `H` dopinam do stringa skrót klawiszy - ten sam mechanizm.


## Jak to działa 

Kolejność w programie jest taka:

1. GLFW trzyma okno i zbiera klawisze/mysz.
2. GLAD ładuje funkcje OpenGL.
3. Co klatkę:
   - ruszam ciała testowe na CPU,
   - wysyłam parametry do shadera (czas, masa, pozycja kamery, flagi...),
   - rysuję ten jeden prostokąt na całe okno (obraz czarnej dziury),
   - na wierzchu dorysowuję napisy HUD (trójkąty ze `stb_easy_font`).

Vertex shader (`shaders/fullscreen.vert`) prawie nic nie robi - tylko ustawia narożniki prostokąta.
Cały obraz czarnej dziury liczy fragment shader (`shaders/blackhole.frag`). Dla każdego piksela:

1. Z pozycji piksela buduję kierunek "promienia światła" wychodzącego z kamery.
2. Idę tym promieniem małymi krokami w stronę dziury (całkowanie numeryczne, Verlet).
3. Przy każdym kroku dokłada się zagięcie - wzór z formy Bineta (Schwarzschild, uproszczony):
   `a = -1.5 * h² * x / r⁵`
   Klawisz `L` to wyłącza. Wtedy to jest to samo ujęcie ale bez zgiętego światła.
4. Jak promień przetnie płaszczyznę dysku w pierścieniu (między wewnętrznym a zewnętrznym
   promieniem), dodaję kolor dysku. Opcjonalnie Doppler: jedna strona jaśniejsza/czerwieńsza.
5. Jak wpadnie zbyt blisko środka (pod horyzont zdarzeń) - piksel wychodzi czarny.
6. Jak ucieknie daleko - w tle widać gwiazdy (nie z tekstury, tylko wyliczone w shaderze).

W shaderze horyzont ma promień 1. Masa z klawiszy `+/-` skaluje kamerę względem tego,
więc większa masa = mocniejsze zagięcie i "większa" dziura na ekranie.

Jeszcze raz: to model wizualny, nie pełna OTW. Szczegóły i linki są w `docs/`.


## Co siedzi w plikach

```
src/main.cpp      - start programu, pętla, klawisze/mysz, wysyłanie parametrów, screenshot
src/shader.*      - wczytanie plików GLSL i kompilacja na GPU
src/camera.*      - obrót i zoom kamery
src/particles.*   - orbity ciał testowych na CPU
src/hud.*         - napisy: stb_easy_font -> trójkąty -> prosty shader
shaders/fullscreen.vert   - ten prostokąt na całe okno
shaders/blackhole.frag    - liczenie obrazu czarnej dziury
docs/             - docs użytkownika, techniczna, źródła
external/         - GLAD, stb, zapasowe nagłówki jak system czegoś nie ma
```

## Sterowanie (przy demie)

- lewy przycisk myszy - obrót, scroll - zoom
- `+/-` - masa (siła soczewki)
- `9`/`0` (albo `[` `]`) - prędkość czasu, Spacja - pauza
- `Q` - jakość 1-2-3, `D` - Doppler, `L` - soczewkowanie włącz/wyłącz
- `P` - ciała testowe, srodkowy przycisk myszy - dokłada kolejne (max 24)
- `U` `I` - wewnętrzna krawędź dysku, `T` `Y` - przechylenie dysku
- `V` - gotowe ujęcie, `A` - kamera sama się kręci
- `F` - pełny ekran, `F12` - zrzut do pliku `.ppm`
- `H` - help, `R` - reset, `Esc` - wyjście

## Budowanie

```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev libxinerama-dev
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Bez paczek GLFW/glm CMake pobierze je sam (sieć). Potrzebny też sterownik OpenGL.
Jak brakuje Xineramy, jest lokalny folder `external/x11_extra`.

Opcjonalnie: `./blackhole --selftest` sprawdza logikę klawiszy bez otwierania okna.


## Skąd wzielam rzeczy

Główna inspiracja: Shadertoy https://www.shadertoy.com/view/tsBXW3 (Black Hole Accretion).
Wzór na zagięcie światła: forma Bineta dla fotonu w metryce Schwarzschilda (uproszczona).
Podglądałam też Erica Brunetona (u niego sa gotowe tablice, u mnie natomiast jest liczenie krok po kroku)
i inne dema black hole - lista w `docs/zrodla.md`.

Biblioteki: GLFW (okno), GLAD (OpenGL), glm (wektory), stb_easy_font (tekst), CMake.


## Docs

- `docs/uzytkownik.md` - jak odpalić i sterować
- `docs/techniczna.md` - więcej o potoku i wzorach
- `docs/zrodla.md` - linki
- `plan-projektu.md` - pierwotny plan

Screeny: `docs/screenshots/` (default, orbit, zoom).
