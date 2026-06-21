/* 
 * Thils file is stitched together from Niko's bigduckgl.
 * Originally used for LGDV's Grapa.
 *
 */
#include "context.h"
// #include "file_util.h"
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>

#include "config.h"

using namespace std;

// -------------------------------------------
// helper funcs

static void glfw_error_func(int error, const char *description) {
    fprintf(stderr, "GLFW: Error %i: %s\n", error, description);
}

static void parse_gl_header(); // implementation below

static void (*user_keyboard_callback)(int key, int scancode, int action, int mods) = 0;
static void (*user_mouse_callback)(double xpos, double ypos) = 0;
static void (*user_mouse_button_callback)(int button, int action, int mods) = 0;
static void (*user_mouse_scroll_callback)(double xoffset, double yoffset) = 0;
static void (*user_resize_callback)(int w, int h) = 0;

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, 1);
    if (user_keyboard_callback)
        user_keyboard_callback(key, scancode, action, mods);
}

static void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (user_mouse_callback)
        user_mouse_callback(xpos, ypos);
}

static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (user_mouse_button_callback)
        user_mouse_button_callback(button, action, mods);
}

static void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (user_mouse_scroll_callback)
        user_mouse_scroll_callback(xoffset, yoffset);
}

static void glfw_resize_callback(GLFWwindow* window, int w, int h) {
    Context::resize(w, h);
    if (user_resize_callback)
        user_resize_callback(w, h);
}


static void glfw_char_callback(GLFWwindow* window, unsigned int c) {
}

// -----------------------------------------------------------
// GL debug output

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    // get source of error
    string src;
    switch (source){
    case GL_DEBUG_SOURCE_API:
        src = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        src = "WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        src = "SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        src = "THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:
        src = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:
        src = "OTHER"; break;
    }
    // get type of error
    string typ;
    switch (type){
    case GL_DEBUG_TYPE_ERROR:
        typ = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typ = "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typ = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typ = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typ = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER:
        typ = "OTHER"; break;
    case GL_DEBUG_TYPE_MARKER:
        typ = "MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typ = "PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typ = "POP_GROUP"; break;
    }
    // get severity
    string sev;
    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        sev = "NOTIFICATION"; break;
    case GL_DEBUG_SEVERITY_LOW:
        sev = "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        sev = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:
        sev = "HIGH"; break;
    }
    fprintf(stderr, "GL_DEBUG: Severity: %s, Source: %s, Type: %s.\nMessage: %s\n", sev.c_str(), src.c_str(), typ.c_str(), message);
}

void enable_gl_debug_output() {
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(debugCallback, 0);
    //disable_gl_notifications();
}

void disable_gl_debug_output() {
    glDisable(GL_DEBUG_OUTPUT);
}

void enable_gl_notifications() {
    //glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_TRUE);
}

void disable_gl_notifications() {
    glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
}

// -------------------------------------------
// Context

static ContextParameters parameters; std::string Context::base_path;

Context::Context() {
    if (!glfwInit()) {
        std::cerr << "glfwInit failed!" << std::endl;
        exit(1);
    }
    glfwSetErrorCallback(glfw_error_func);

    // some GL context settings
    if (parameters.gl_major > 0)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, parameters.gl_major);
    if (parameters.gl_minor > 0)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, parameters.gl_minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, parameters.resizable);
    glfwWindowHint(GLFW_VISIBLE, parameters.visible);
    glfwWindowHint(GLFW_DECORATED, parameters.decorated);
    glfwWindowHint(GLFW_FLOATING, parameters.floating);
    glfwWindowHint(GLFW_MAXIMIZED, parameters.maximised);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, parameters.gl_debug_context);
	vp_w = parameters.width;
	vp_h = parameters.height;

    // create window and context
    glfw_window = glfwCreateWindow(parameters.width, parameters.height, parameters.title.c_str(), 0, 0);
    if (!glfw_window) {
        std::cerr << "ERROR: glfwCreateContext failed!" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(parameters.swap_interval);

    glewExperimental = GL_TRUE;
    const GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "ERROR: GLEWInit failed: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(glfw_window);
        glfwTerminate();
        exit(1);
    }
    // Neu hinzufügen:
    glGetError(); // GLEW erzeugt manchmal einen harmlosen Fehler auf Mac, den wir leeren
    // output configuration
    std::cerr << "GLFW: " << glfwGetVersionString() << std::endl;
    std::cerr << "OpenGL: " << glGetString(GL_VERSION) << ", " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // enable debugging output
    enable_gl_debug_output();

    // setup user ptr
    glfwSetWindowUserPointer(glfw_window, this);

    // install callbacks
    glfwSetKeyCallback(glfw_window, glfw_key_callback);
    glfwSetCursorPosCallback(glfw_window, glfw_mouse_callback);
    glfwSetMouseButtonCallback(glfw_window, glfw_mouse_button_callback);
    glfwSetScrollCallback(glfw_window, glfw_mouse_scroll_callback);
    glfwSetFramebufferSizeCallback(glfw_window, glfw_resize_callback);
    glfwSetCharCallback(glfw_window, glfw_char_callback);

    // set input mode
    glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(glfw_window, GLFW_STICKY_MOUSE_BUTTONS, 1);

    // init GLenum <-> string mapping
    parse_gl_header();

    // set some sane GL defaults
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1);

    // setup timer
    last_t = curr_t = glfwGetTime();
}

Context::~Context() {
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

Context& Context::init(const ContextParameters& params) {
    parameters = params;
    return instance();
}

Context& Context::instance() {
    static Context ctx;
    return ctx;
}

bool Context::running() { return !glfwWindowShouldClose(instance().glfw_window); }

void Context::swap_buffers() {
    glfwSwapBuffers(instance().glfw_window);
    instance().last_t = instance().curr_t;
    instance().curr_t = glfwGetTime() * 1000; // s to ms
    glfwPollEvents();
}

double Context::frame_time() { return instance().curr_t - instance().last_t; }

void Context::show() { glfwShowWindow(instance().glfw_window); }

void Context::hide() { glfwHideWindow(instance().glfw_window); }

glm::ivec2 Context::resolution() {
    int w, h;
    glfwGetFramebufferSize(instance().glfw_window, &w, &h);
    return glm::ivec2(w, h);
}

void Context::resize(int w, int h) {
    glfwSetWindowSize(instance().glfw_window, w, h);
    glViewport(0, 0, w, h);
	instance().vp_w = w;
	instance().vp_h = h;
}

void Context::set_title(const std::string& name) { glfwSetWindowTitle(instance().glfw_window, name.c_str()); }

void Context::set_swap_interval(uint32_t interval) { glfwSwapInterval(interval); }

void Context::capture_mouse(bool on) { glfwSetInputMode(instance().glfw_window, GLFW_CURSOR, on ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL); }

glm::vec2 Context::mouse_pos() {
    double xpos, ypos;
    glfwGetCursorPos(instance().glfw_window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}

bool Context::mouse_button_pressed(int button) { return glfwGetMouseButton(instance().glfw_window, button) == GLFW_PRESS; }

bool Context::key_pressed(int key) { return glfwGetKey(instance().glfw_window, key) == GLFW_PRESS; }

void Context::set_keyboard_callback(void (*fn)(int key, int scancode, int action, int mods)) { user_keyboard_callback = fn; }

void Context::set_mouse_callback(void (*fn)(double xpos, double ypos)) { user_mouse_callback = fn; }

void Context::set_mouse_button_callback(void (*fn)(int button, int action, int mods)) { user_mouse_button_callback = fn; }

void Context::set_mouse_scroll_callback(void (*fn)(double xoffset, double yoffset)) { user_mouse_scroll_callback = fn; }

void Context::set_resize_callback(void (*fn)(int w, int h)) { user_resize_callback = fn; }

// -------------------------------------------
// Enum <-> string conversion

static std::map<std::string, GLint> string_enum_map;
static std::map<GLint, std::string> enum_string_map;

static std::vector<std::string> tokenize(const std::string& str, const std::string& delim) {
    std::vector<std::string> out;
    size_t start, end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find_first_of(delim, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}

inline std::string concat(const std::string& dir, const std::string& file) {
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    const std::string a = dir.back() == '/' || dir.back() == '\\' ?  dir.substr(0, dir.size() - 1) : dir;
    const std::string b = file.front() == '/' || file.front() == '\\' ?  file.substr(1) : file;
    return a + '/' + b;
}

inline std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    return std::string();
}

static void parse_gl_header() {
    std::stringstream input(read_file(concat(OPENGL_INCLUDE_PATH, "GL/glew.h")) + read_file(concat(OPENGL_INCLUDE_PATH, "GL/gl.h")));
    std::string line;
    while (getline(input, line)) {
        if (line.find("#define ") != std::string::npos) {
            line = line.substr(std::string("#define ").size());
            const auto& token = tokenize(line, " \t");
            if (token.size() == 2) {
                try {
                    const std::string s = token[0];
                    const GLint value = std::stoi(token[1], 0, 0);
                    string_enum_map[s] = value;
                    enum_string_map[value] = s;
                } catch (std::exception& e) {}
            }
        }
    }
}

std::string Context::enum_to_string(GLint value) {
    return enum_string_map.at(value);
}

GLint Context::string_to_enum(const std::string& value) {
    return string_enum_map.at(value);
}
