#include "my_math.h"

#include <raymath.h>

bool CheckCollisionPointRotatedRect(Vector2 point, Vector2 rectCenter, float width, float height, float rotationDeg) {
	// translate point into rectangle-centered space
	Vector2 local = Vector2Subtract(point, rectCenter);

	// rotate point by -rotation (undo the rectangle's rotation)
	float rad = -rotationDeg * DEG2RAD;
	float cosR = cosf(rad);
	float sinR = sinf(rad);

	Vector2 rotated = {
		local.x * cosR - local.y * sinR,
		local.x * sinR + local.y * cosR
	};

	// now it's a plain axis-aligned box check
	return (fabsf(rotated.x) <= width / 2.0f) && (fabsf(rotated.y) <= height / 2.0f);
}
