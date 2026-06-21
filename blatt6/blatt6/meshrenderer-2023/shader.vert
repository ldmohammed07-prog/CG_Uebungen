//#version 130
#version 410 core
in vec3 local_vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {

	// TODO: Aufgabe 1.4
	// local_vertex soll im Eye-Space verstanden werden

	// TODO: Aufgabe 1.5
	// local_vertex soll im World-Space verstanden werden

	gl_Position = proj * view * model * ec4(local_vertex, 1.0);
}
