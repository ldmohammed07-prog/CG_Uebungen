#version 150
#define USE_PHONG_SHADING

in vec3 in_pos;
in vec3 in_norm;

uniform mat4 model;
uniform mat4 model_normal;
uniform mat4 view;
uniform mat4 view_normal;
uniform mat4 proj;

#ifdef USE_PHONG_SHADING
// {{{
out vec3 pos_ws;
out vec3 n_ws;
// }}}
#else
#endif


void main() {
#ifdef USE_PHONG_SHADING
	// {{{
	n_ws = normalize(mat3(model_normal) * in_norm);
	pos_ws = (model * vec4(in_pos, 1.0)).xyz;
	// }}}
#else
	// TODO (optional) Phong Lighting mit Gouraud Shading
#endif
	gl_Position = proj * view * model * vec4(in_pos, 1.0);
}
