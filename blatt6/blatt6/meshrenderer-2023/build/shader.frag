//#version 130
#version 410 core
in vec3 frag_color; // kommt vom Vertex Shader
out vec4 out_color;

void main() {
	out_color = vec4(frag_color, 1);
}
