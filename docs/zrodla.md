# Zrodla

Bazowo Shadertoy, ale to nie jest kopiuj-wklej – wejscia podmienione na wlasne uniformy pod GLFW/OpenGL 3.3.

Glownie patrzylam na:
https://www.shadertoy.com/view/tsBXW3 - Black Hole Accretion (dysk + zagniecie)

`iTime` → `uTime`, `iResolution` → `uResolution`, kamera → `uCamPos` + orbit w C++.

Wzory: forma Bineta dla fotonu w Schwarzschildzie `a = -3/2 h² x / r⁵`.  
Eric Bruneton – Real-time High-Quality Rendering of Non-Rotating Black Holes (u niego LUT, u mnie numerycznie): https://ebruneton.github.io/black_hole_shader/  
Neil Bickford – Computational Black Holes + https://www.shadertoy.com/view/4s3SzN

Biblioteki: GLFW, GLAD, glm, stb_easy_font (HUD), CMake. Z ksiazek OpenGL (Redbook / SuperBible) jak cos sie przyda przy opisie potoku.
