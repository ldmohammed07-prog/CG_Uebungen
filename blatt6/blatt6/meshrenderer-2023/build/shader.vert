//#version 130
#version 410 core
layout (location = 0) in vec3 local_vertex;
layout (location = 1) in vec3 farb; // kommt aus Farb-VBO
out vec3 frag_color; // weiterleiten an Fragment Shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	frag_color = farb;  // weiterleiten an Fragment Shader

	// TODO: Aufgabe 1.4
	// local_vertex soll im Eye-Space verstanden werden

	// TODO: Aufgabe 1.5
	// local_vertex soll im World-Space verstanden werden

	gl_Position = proj * view * model * vec4(local_vertex, 1.0);
}
