#include "raster.h"

#include <glm/gtc/random.hpp>

using namespace glm;
using namespace png;
using namespace std;

bool overlaps_pixel(const vec2 &p, const triangle &tri, render_target &target);

vector<rgb_pixel> color_palette;
		float triangle_area(vec2 a, vec2 b, vec2 c) {
    	return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y);
}
render_target raster_triangles(const vector<triangle> &triangles)
{
	// set up color palette to visualize different triangles
	color_palette.resize(triangles.size());
	for (int i = 0; i < triangles.size(); ++i)
		color_palette[i] = rgb_pixel(linearRand(0,255), linearRand(0,255), linearRand(0,255));

	// set up render target and clear to dark grey
	render_target target;
	target.framebuffer.resize(cmdline.vp_w, cmdline.vp_h);
	target.depthbuffer.resize(cmdline.vp_h, std::vector<float>(cmdline.vp_w, 1e10f));  // neu!
	for (png::uint_32 y = 0; y < cmdline.vp_h; ++y)
		for (png::uint_32 x = 0; x < cmdline.vp_w; ++x)
			target.framebuffer[y][x] = png::rgb_pixel(60,60,60);

	// go over all triangles 
	for (int i = 0; i < triangles.size(); ++i) {
		const triangle &tri = triangles[i];
		// find bounding box
		vec3 bb_min, bb_max;
		bb_min = floor(min(tri.a, min(tri.b, tri.c)));
		bb_max = ceil(max(tri.a, max(tri.b, tri.c)));
		// clip bounding box
		bb_min.x = fmaxf(bb_min.x, 0.0f);
		bb_min.y = fmaxf(bb_min.y, 0.0f);
		bb_max.x = fminf(bb_max.x, cmdline.vp_w-1.0f);
		bb_max.y = fminf(bb_max.y, cmdline.vp_h-1.0f);
		// go over all pixels in bounding box
		// NOTE: here, we assume to be in window coordinates, NOT NDC

		for (int y = int(bb_min.y); y < int(bb_max.y); ++y)
			for (int x = int(bb_min.x); x < int(bb_max.x); ++x) {

				if (overlaps_pixel(vec2(x+0.5f, y+0.5f), tri, target)) {
					// int inv_y = target.framebuffer.get_height()-1-y;
					// target.framebuffer[inv_y][x] = color_palette[i];
				//float z = tri.a.z;
vec2 a2 = vec2(tri.a);
vec2 b2 = vec2(tri.b);
vec2 c2 = vec2(tri.c);
vec2 p2 = vec2(x+0.5f, y+0.5f);

float total = triangle_area(a2, b2, c2);
float alpha = triangle_area(p2, b2, c2) / total;
float beta  = triangle_area(a2, p2, c2) / total;
float gamma = 1.0f - alpha - beta;
float z = alpha * tri.a.z + beta * tri.b.z + gamma * tri.c.z;
//cout << "z = " << z << endl;  // debug
if (y >= 0 && y < cmdline.vp_h && x >= 0 && x < cmdline.vp_w) {
if (z < target.depthbuffer[y][x]) {
    target.depthbuffer[y][x] = z;
    int inv_y = target.framebuffer.get_height()-1-y;
    target.framebuffer[inv_y][x] = color_palette[i];
}
}		
				}
			}
	}

	return target;
}


vec2 line_normal(vec2 a, vec2 b)
{
	vec2 v = normalize(b-a);
	return vec2(-v.y, v.x);
}

bool overlaps_pixel(const vec2 &p, const triangle &tri, render_target &target)
{
	if (dot(line_normal(tri.a, tri.b), p - vec2(tri.a)) < 0) return false;
	if (dot(line_normal(tri.b, tri.c), p - vec2(tri.b)) < 0) return false;
	if (dot(line_normal(tri.c, tri.a), p - vec2(tri.c)) < 0) return false;

	if (cmdline.pipeline != window_only)
		if (tri.a.z < 0 || tri.b.z < 0 || tri.c.z < 0)
			return false;

	return true;
}
