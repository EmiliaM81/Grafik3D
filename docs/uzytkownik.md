# Dokumentacja użytkownika

Program pokazuje czarną dziurę z dyskiem akrecyjnym i zakrzywionym tłem (soczewkowanie). Da się kręcić kamerą i zmieniać kilka parametrów.

## Uruchomienie

Na Linuxie (Ubuntu/Debian mniej więcej):

```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev libxinerama-dev
mkdir -p build && cd build
cmake ..
cmake --build .
./blackhole
```

Jak nie ma GLFW/glm w systemie, CMake ściągnie je sam (trzeba sieci przy pierwszym `cmake ..`).

## Sterowanie

- LMB + przeciąganie – obrót kamery wokół dziury
- scroll – zoom
- `+` / `-` – masa / siła soczewkowania
- `[` / `]` – prędkość czasu (animacja dysku i cząstek)
- `1` / `2` / `3` – jakość (mniej / normalnie / więcej kroków w shaderze)
- `D` – włącza/wyłącza prosty Doppler na dysku
- `P` – pokazuje/ukrywa orbitujące cząstki
- `R` – reset kamery i parametrów
- `Esc` – wyjście

Parametry przy zmianie wypisują się też w terminalu.
