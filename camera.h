//
// Created by Sagiv Marzini on 02/10/2026.
//

#ifndef CSHOOTER_CAMERA_H
#define CSHOOTER_CAMERA_H
#include <raylib.h>

void camera_init(void);

Camera2D* camera_get(void);

Vector2 camera_screen_to_world(Vector2 screenPos);

#endif //CSHOOTER_CAMERA_H
