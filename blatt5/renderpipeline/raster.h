#pragma once

#include "scene.h"

#include <png++/png.hpp>

struct render_target {
	png::image<png::rgb_pixel> framebuffer;
	std::vector<std::vector<float>> depthbuffer;  // neu!
};


render_target raster_triangles(const std::vector<triangle> &triangles);
