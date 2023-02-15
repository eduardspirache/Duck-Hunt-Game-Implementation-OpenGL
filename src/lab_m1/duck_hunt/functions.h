#pragma once

#include <time.h>
#include <cstdlib>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


float module(float number) {
	return number > 0 ? number : -1 * number;
}

float pos_neg() {
	int seed = rand() % 2 + 1;
	return seed == 1 ? -1 : 1;
}

void random_spawn(glm::vec2 &spawn_position, float &angle, glm::ivec2 resolution) {

	float x = rand() % resolution.x;
	spawn_position = glm::vec2(x, 0);

	
	angle = rand() % 85;
	if (angle < 15)
		angle += 20;

	if (x < resolution.x / 4) {
		angle = module(angle);
	} 
	else if (x > 3 * (resolution.x / 4)) {
		angle = - module(angle);
	}
	angle *= (3.14159265359 / 180);
}

int signof(float number) {
	return number > 0 ? 1 : -1;
}

void hit(glm::vec2 current_position, glm::vec2& direction_vector, float& direction_angle, float& rotation_angle, int& hit_count, glm::ivec2 resolution, glm::vec2 &hitbox) {
	if (resolution.x < current_position[0] || 0 > current_position[0]) {
		direction_vector[0] *= -1;
		hitbox[1] *= -1;
		rotation_angle = 3.14159265359 - rotation_angle + 3.14159265359;
	}
	else if (resolution.y < current_position[1] || 0 > current_position[1]) {
		direction_vector[1] *= -1;
		hitbox[0] *= -1;
		rotation_angle = 3.14159265359 - rotation_angle;
	}
}
