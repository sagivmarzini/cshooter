#include <raylib.h>

#include "common.h"

static Camera2D g_camera;

void camera_init(void) {
	g_camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
	g_camera.zoom = 1.0f;
}

Camera2D* camera_get(void) { return &g_camera; }

Vector2 camera_screen_to_world(Vector2 screenPos) {
	return GetScreenToWorld2D(screenPos, g_camera);
}
