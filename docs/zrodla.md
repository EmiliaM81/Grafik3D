# Źródła

Shader nie jest kopiuj-wklej jednego pliku 1:1. Port = własny kod pod GLFW/OpenGL 3.3, ale fizyka wizualna i układ jak w typowych demo Shadertoy.

## Shadertoy / dema

Główna inspiracja techniczna (całkowanie geodezyjnej + przecięcia dysku, wygląd Interstellar):

- https://www.shadertoy.com/view/tsBXW3 – „Black Hole Accretion” (akrecja + zagięcie)

Podobne podejście widać w wielu innych black hole na Shadertoy (ta sama forma przyspieszenia fotonu).

Wejścia Shadertoy podmienione na własne uniformy:
`iTime` → `uTime`, `iResolution` → `uResolution`, mysz/kamera → `uCamPos` + orbit w C++.

## Wzory / opisy

- Forma Bineta dla fotonu w Schwarzschildzie: `a = -3/2 h² x / r⁵` (często w tutorialach i kodach RT wokół czarnej dziury).
- Eric Bruneton – „Real-time High-Quality Rendering of Non-Rotating Black Holes” (opis LUT / geodezyjnych; u nas zamiast LUT jest numeryczne całkowanie): https://ebruneton.github.io/black_hole_shader/
- Neil Bickford – notatki „Computational Black Holes…” + Shadertoy https://www.shadertoy.com/view/4s3SzN (kontekst edukacyjny)

## Biblioteki

- GLFW, GLAD, glm, CMake – standard pod OpenGL na Linuxie
- OpenGL Programming Guide / SuperBible – jak coś z potoku wejdzie do prezentacji

## Licencja / autorzy

Przy prezentacji i w docs trzeba wymienić Shadertoy (tsBXW3) jako inspirację wizualną/techniczną oraz że implementacja w repo jest własnym portem pod ten projekt zaliczeniowy.
