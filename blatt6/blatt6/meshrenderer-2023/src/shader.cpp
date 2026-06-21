#include "shader.h"

#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// This is from bigduckgl
static std::string get_log(GLuint object) {
    std::string error_string;
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        error_string += "Not a shader or a program";
        return error_string;
    }
    if (log_length <= 1)
        // ignore empty string
        return error_string;
    char *log = (char *)malloc(log_length);
    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);
    error_string += log;
    free(log);
    return error_string;
}

static std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    return std::string();
}


static GLuint shader_prog = -1;

void load_shader() {
	shader_prog = glCreateProgram();
	
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vert_source = read_file("shader.vert");
	const char *vert_source_cstr = vert_source.c_str();
	glShaderSource(vert_shader, 1, &vert_source_cstr, 0);
	glCompileShader(vert_shader);
	GLint result = GL_FALSE;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);

	if (result != GL_TRUE) {
		cerr << "Failed to compile shader 'shader.vert'" << endl;
		cerr << get_log(vert_shader); 
		glDeleteShader(vert_shader);
		glDeleteProgram(shader_prog);
		shader_prog = -1;
		exit(-1);
	}
	glAttachShader(shader_prog, vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string frag_source = read_file("shader.frag");
	const char *frag_source_cstr = frag_source.c_str();
	glShaderSource(frag_shader, 1, &frag_source_cstr, 0);
	glCompileShader(frag_shader);
	result = GL_FALSE;
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);

	if (result != GL_TRUE) {
		cerr << "Failed to compile shader 'shader.frag'" << endl;
		cerr << get_log(frag_shader); 
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		glDeleteProgram(shader_prog);
		shader_prog = -1;
		exit(-1);
	}
	glAttachShader(shader_prog, frag_shader);

	glLinkProgram(shader_prog);
	result = GL_FALSE;
	glGetProgramiv(shader_prog, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		cerr << "Failed to link shaders 'shader.vert' and 'shader.frag'" << endl;
		cerr << get_log(shader_prog); 
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		glDeleteProgram(shader_prog);
		shader_prog = -1;
		exit(-1);
	}
}

void bind_shader() {
	glUseProgram(shader_prog);
}

void unbind_shader() {
	glUseProgram(0);
}

int uniform_location(const std::string &name) {
	int loc = glGetUniformLocation(shader_prog, name.c_str());
	return loc;
}
