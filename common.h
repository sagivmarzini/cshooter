#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define GAME_FPS 60
#define MAX_BULLETS 100

typedef struct {
	int health;
	Vector2 pos;
	Vector2 vel;
	float rotation;
	bool active;
} Entity;

#endif // COMMON_H
