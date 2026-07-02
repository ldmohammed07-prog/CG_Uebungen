#version 410
#define USE_PHONG_SHADING

// Explizite Attribut-Locations: die Mesh bindet Buffer fest an
// Location 0 = Position, Location 1 = Normale (mesh.cpp). Ohne diese
// Angabe wuerde der Treiber die Locations frei vergeben -> auf Apple-GL
// landet in_pos evtl. auf dem Normalen-Buffer -> Geometrie kaputt.
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;

uniform mat4 model;
uniform mat4 model_normal;
uniform mat4 view;
uniform mat4 view_normal;
uniform mat4 proj;

#ifdef USE_PHONG_SHADING
// {{{
// --- WORLDSPACE-VARIANTE (aktiv) ---
out vec3 pos_ws;
out vec3 n_ws;

// --- EYESPACE-VARIANTE (Alternative) ---
// Position und Normale in Kamerakoordinaten (Eyespace) an den Fragment-Shader geben.
//out vec3 pos_es;
//out vec3 n_es;
// }}}
#else
#endif


void main() {
#ifdef USE_PHONG_SHADING
	// {{{
	// --- WORLDSPACE-VARIANTE (aktiv) ---
	n_ws = normalize(mat3(model_normal) * in_norm);
	pos_ws = (model * vec4(in_pos, 1.0)).xyz;

	// --- EYESPACE-VARIANTE (Alternative) ---
	// view * model bringt die Position in den Eyespace;
	// view_normal * model_normal transformiert die Normale entsprechend.
	//n_es   = normalize(mat3(view_normal) * mat3(model_normal) * in_norm);
	//pos_es = (view * model * vec4(in_pos, 1.0)).xyz;
	// }}}
#else
	// TODO (optional) Phong Lighting mit Gouraud Shading
#endif
	gl_Position = proj * view * model * vec4(in_pos, 1.0);
}
