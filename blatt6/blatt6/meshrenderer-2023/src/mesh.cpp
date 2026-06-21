#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <GL/glew.h>  //❌ Linux
#include <OpenGL/gl.h>

using namespace std;
using namespace glm;

vector<vec3> vertices;
vector<unsigned> indices;

void add_tri(int a, int b, int c) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
}

void add_vert(float x, float y, float z,
			  float, float, float) {
	vertices.push_back({x,y,z});
}

static GLuint vao = -1;
static GLuint vbo = -1;
static GLuint vbo_col = -1;
static GLuint ibo = -1;

//#define CS_TRI
//#define ES_TRI
#define BUNNY

#define VAO

void load_mesh() {
#ifdef BUNNY
	#include "bunny.data"
#elif defined CS_TRI
	add_vert(-1,-1,0,0,0,0);
	add_vert( 1,-1,0,0,0,0);
	add_vert(0,1,0,0,0,0);
#else
	add_vert( 0, 0,-10,0,0,0);
	add_vert(10, 0,-10,0,0,0);
	add_vert( 0,10,-10,0,0,0);
#endif
	add_tri(0,1,2);
	// TODO: Aufgabe 1.3
	// VAO aufsetzen

// 1. VAO erstellen und aktivieren
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);

// 2. VBO erstellen und aktivieren
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);	

// 3. Vertex Attribut verbinden
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

// 4. IBO erstellen und aktivieren
glGenBuffers(1, &ibo);
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

vector<vec3> colors;
for (int i = 0; i < vertices.size(); i +=3) {
	vec3 farbe = glm::linearRand(vec3(0), vec3(1));
	colors.push_back(farbe);
	colors.push_back(farbe);
	colors.push_back(farbe);
}
// 3. VBO für Farben (1 Farbe pro einzigartigem Vertex!)
// vector<vec3> colors;
// for (int i = 0; i < vertices.size(); i++) {
//     colors.push_back(glm::linearRand(vec3(0), vec3(1)));
// }
glGenBuffers(1, &vbo_col);
glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);

glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

// 5. VAO deaktivieren
glBindVertexArray(0);

}

void draw_mesh() {
	// TODO: Aufgabe 1.3
	// VAO rendern
	glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		// Zeichne Dreiecke, beginne bei Punkt 0, zeichne alle Punkte
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

}

