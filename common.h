#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define GAME_FPS 60

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float rotation;
    float radius;
    bool active;
} Entity;

#endif // COMMON_H
