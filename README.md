# PlayGame3

Una coleccion de 5 juegos clasicos con graficos vectoriales para PlayStation 3, desarrollados con PSL1GHT SDK y C++.

## Juegos incluidos

| Juego | Descripcion |
|-------|-------------|
| **PONG** | Ping pong clasico 1 jugador vs IA. D-PAD para mover, X para servir. |
| **SNAKE** | Snake clasico en grid 40x30. D-PAD para dirigir, come y crece. |
| **ASTEROIDS** | Asteroids con fisicas inerciales y poligonos vectoriales. D-PAD rotar/acelerar, X disparar. |
| **BREAKOUT** | Rompe ladrillos con 3 tipos de resistencias. D-PAD mover, X servir. |
| **SPACE INVADERS** | Invasores del espacio con 3 tipos de aliens. D-PAD mover, X disparar. |

## Requisitos

- PS3DEV toolchain (ppu-g++, make_self, make_pkg)
- PSL1GHT SDK

## Compilacion

```bash
export PS3DEV=/usr/local/ps3dev
export PSL1GHT=$PS3DEV
export PATH=$PATH:$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin

make        # Compila playgame3.self
make pkg    # Empaqueta playgame3.pkg
make clean  # Limpia archivos temporales
```

## GitHub Actions

El proyecto incluye un workflow de GitHub Actions que:

1. Construye automaticamente el toolchain PS3DEV desde fuente (con cache)
2. Compila el juego y genera `.self`
3. Empaqueta en `.pkg`
4. Sube ambos como artefactos

> La primera compilacion toma ~45-60 min (toolchain). Siguientes compilaciones usan cache y toman ~2 min.

## Estructura del proyecto

```
src/
  main.cpp          - Punto de entrada y loop principal
  renderer.h/cpp    - Motor grafico vectorial via RSX/GCM
  input.h/cpp       - Manejo de controles PS3
  font.h            - Font bitmap 8x8
  menu.h/cpp        - Menu principal del juego
  games/
    game.h           - Interfaz base para juegos
    pong.h/cpp       - Juego Pong
    snake.h/cpp      - Juego Snake
    asteroids.h/cpp  - Juego Asteroids
    breakout.h/cpp   - Juego Breakout
    spaceinvaders.h/cpp - Juego Space Invaders
```

## Controles

- **D-PAD**: Navegar / Mover
- **X**: Seleccionar / Disparar / Servir
- **CIRCLE**: Salir del juego actual
- **START**: Seleccionar en menu

## Licencia

BSD-2-Clause
