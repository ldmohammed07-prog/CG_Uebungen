#include "mesh.h"
#include "shader.h"
#include "context.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <OpenGL/gl.h>

using namespace std;
using namespace glm;


mat4 perspective_projection_transform(float fovy, float aspect, float n, float f) {
	mat4 P(0);
	
	// TODO: Aufgabe 1.4
	// OpenGL Projektionsmatrix
	float h = n* glm::tan(glm::radians(fovy/2));
	float w = h * aspect;
	P[0][0] = n/w;
	P[1][1] = n/h;
	P[2][2] = -(f+n)/(f-n);
	P[2][3] = -1;
	P[3][2] = -2*f*n/(f-n);
	

	return P;
}

mat4 viewing_transform(const vec3 &pos, const vec3 &dir, const vec3 &up) {
	
	// TODO: Aufgabe 1.5
	// OpenGL Viewingmatrix
	vec3 w = -glm::normalize(dir);
	vec3 u = glm::normalize(glm::cross(up, w));
	vec3 v = glm::cross(w, u);	
mat4 R(1);
R[0][0] = u.x; R[1][0] = u.y; R[2][0] = u.z;
R[0][1] = v.x; R[1][1] = v.y; R[2][1] = v.z;
R[0][2] = w.x; R[1][2] = w.y; R[2][2] = w.z;

mat4 T(1);
T[3][0] = -pos.x;
T[3][1] = -pos.y;
T[3][2] = -pos.z;

mat4 V = R * T;
	return V;
}


int main() {
	ContextParameters params;
	params.gl_major = 3;
	params.gl_minor = 2;
	params.title = "CG'20 mini renderer";
	Context::init(params);

	load_mesh();
	load_shader();

	while (Context::running()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bind_shader();
		float aspect = float(Context::instance().vp_w) / Context::instance().vp_h;
		mat4 P = perspective_projection_transform(65, aspect, 1, 300);
		//mat4 V = viewing_transform(vec3(180,0,50), vec3(-1, 0, 0), vec3(0,0,1));
		// --- NEUE ROTIERENDE KAMERA ---
		float time = glfwGetTime(); // Echte Zeit in Sekunden abfragen
		float radius = 180.0f;      // Dein aktueller Kamera-Abstand
		
		// Kreisbahn auf der X/Y-Ebene (da bei dir Z "oben" ist)
		float camX = sin(time) * radius;
		float camY = cos(time) * radius;
		
		vec3 camPos = vec3(camX, camY, 50.0f); // Neue Position auf dem Kreis
		vec3 camDir = vec3(0, 0, 50.0f) - camPos; // Blickrichtung immer in die Mitte
		
		mat4 V = viewing_transform(camPos, camDir, vec3(0, 0, 1));
		// ------------------------------
		glUniformMatrix4fv(uniform_location("model"), 1, GL_FALSE, glm::value_ptr(mat4(1)));
		glUniformMatrix4fv(uniform_location("view"), 1, GL_FALSE, glm::value_ptr(V));
		glUniformMatrix4fv(uniform_location("proj"), 1, GL_FALSE, glm::value_ptr(P));
		draw_mesh();
		unbind_shader();

		Context::swap_buffers();
	}

	cout << "All seems to be fine :)" << endl;
	return 0;
}
