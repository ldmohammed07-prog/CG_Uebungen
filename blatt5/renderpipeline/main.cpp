#include "cmdline.h"
#include "scene.h"
#include "raster.h"

#include <iostream>
#include <functional>

// evtl für die Ausgabe nötig
 #define GLM_ENABLE_EXPERIMENTAL
 #include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

mat4 window_transform(int w, int h, float n, float f) {
	// TODO Bestimmen Sie die Window-Transform (aka Viewport Transformation).
	// Da wir das ganze Bild füllen ist der Offset jeweils 0.
	// ACHTUNG. Bei GLM (wie bei OpenGL generell) werden Column-Major Matrizen verwendet!
	// mat4(vec4(1,2,3,4),                         [ 1 5 9 3 ]
	//      vec4(5,6,7,8),    entspricht deshalb   [ 2 6 0 4 ]
	//      vec4(9,0,1,2),                         [ 3 7 1 5 ]
	//      vec4(3,4,5,6));                        [ 4 8 2 6 ]
	// Sie können die Matrizen mit
	//     cout << to_string(V) << endl;
	// ausgeben (beachten Sie dafür auch die beiden auskommentieren Präprozessorzeilen oben)
	//
mat4 W(vec4(w/2,  0,    0,        0),   // Spalte 1
       vec4(0,    h/2,  0,        0),   // Spalte 2
       vec4(0,    0,    (f-n)/2,  0),   // Spalte 3
       vec4(w/2,  h/2,  (f+n)/2,  1)); // Spalte 4

return W;
}

mat4 perspective_projection_transform(float fovy, float aspect, float n, float f) {
	// TODO Geben Sie die Projetionsmatrix in der Fov/Aspect-Variante an
	// Ein Beispiel finden Sie hier: 
	// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
	// Achtung, wie oben beschrieben verwenden wir Column-Major Matrizen.
	//
float t = n * tan(glm::radians(fovy / 2.0f));
float b = -t;
float r = aspect * t;
float l = -r;
	mat4 P(vec4((2*n)/(r-l), 0, 0, 0),
		   vec4(0, (2*n)/(t-b), 0, 0),
		   vec4((r+l)/(r-l), (t+b)/(t-b), -(f+n)/(f-n), -1),
		   vec4(0, 0, -(2*f*n)/(f-n), 0));
	return P;
}

mat4 viewing_transform(const vec3 &pos, const vec3 &dir, const vec3 &up) {
	// TODO Geben sie die View-Matrix an.
	// Die Transformationsrichtung ist von der Welt IN den Eye Space
	// Achtung, wie oben beschrieben verwenden wir Column-Major Matrizen.
	//
	vec3 w = -glm::normalize(dir);
	vec3 u = glm::normalize(glm::cross(up, w));
	vec3 v = glm::cross(w, u);
mat4 R(vec4(u.x, v.x, w.x, 0),
       vec4(u.y, v.y, w.y, 0),
       vec4(u.z, v.z, w.z, 0),
       vec4(0,   0,   0,   1));
	mat4 T(vec4(1,0,0,0),
		   vec4(0,1,0,0),
		   vec4(0,0,1,0),
		   vec4(-pos,1));
	mat4 V = R * T;
	return V;
}

int main(int argc, char **argv)
{
	parse_cmdline(argc, argv);

	vector<triangle> triangles = scene_triangles();

	mat4 W = window_transform(cmdline.vp_w, cmdline.vp_h, cmdline.n, cmdline.f);
	mat4 P = perspective_projection_transform(cmdline.fovy,
											  float(cmdline.vp_w)/cmdline.vp_h,
											  cmdline.n,
											  cmdline.f);
	cout << "P = " << to_string(P) << endl;
	vec4 testpoint = P * vec4(2, 4, -10, 1.0f);
testpoint /= testpoint.w;
cout << "NDC point = " << to_string(testpoint) << endl;
	mat4 V = viewing_transform(cmdline.cam_pos, cmdline.view_dir, cmdline.world_up);

	auto W_transform = [&](const vec3 &v) {
		// TODO Implementieren Sie die Transformation von NDC nach Viewport/Window Koordinaten
		vec4 result = W * vec4(v,1.0f);
		return vec3(result);
	};
	auto PW_transform = [&](const vec3 &v) {
		// TODO Implementieren Sie die Transformation von Eye-Space in Viewport/Window Koordinaten
		vec4 result = P * vec4(v,1.0f);
		result /= result.w; // Homogenes Teilen
		result = W * result;
		return vec3(result);
	};
	auto VPW_transform = [&](const vec3 &v) {
		// TODO Implementieren Sie die Transformation von Weltkoordinaten in Viewport/Window Koordinaten
	vec4 result = V * vec4(v,1.0f);
	result = P * result;
	result /= result.w;
	result = W * result;
		return vec3(result);
	
	};

	function<vec3(const vec3&)> vertex_trasnform;
	if (cmdline.pipeline == window_only)      vertex_trasnform = W_transform;
	if (cmdline.pipeline == proj_window)      vertex_trasnform = PW_transform;
	if (cmdline.pipeline == view_proj_window) vertex_trasnform = VPW_transform;
	
	for (auto &tri : triangles) {
		tri.a = vertex_trasnform(tri.a);
		tri.b = vertex_trasnform(tri.b);
		tri.c = vertex_trasnform(tri.c);
		// cout <<"a: " << to_string(tri.a) << endl;
		// cout <<"b: " << to_string(tri.b) << endl;
		// cout <<"c: " << to_string(tri.c) << endl;
	}
	auto image = raster_triangles(triangles);
	image.framebuffer.write(cmdline.outfile);

	return 0;
}
