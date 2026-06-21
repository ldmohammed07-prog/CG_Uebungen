#pragma once

#include "cmdline.h"

#include <vector>

struct triangle {
	glm::vec3 a, b, c;
};

std::vector<triangle> scene_triangles();
