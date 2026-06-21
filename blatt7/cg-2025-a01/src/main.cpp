#include <bigduckgl/bigduckgl.h>
#include <imgui/imgui.h>

#include <iostream>
#include <vector>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#ifdef CG_KART
#include "cg_kart/scene.h"
#include "cg_kart/kart.h"
#include "cg_kart/physics.h"
#endif

using namespace std;

bool game_is_running = true;

int main(int argc, char** argv) {
//     parse_cmdline(argc, argv);

	// init context and set parameters
	ContextParameters params;
	params.gl_major = 3;
	params.gl_minor = 3;
	params.title = "CG'24";
	params.font_ttf_filename = "render-data/fonts/DroidSansMono.ttf";
	params.font_size_pixels = 15;
	Context::init(params);

	auto cam = make_camera("cam");
	cam->pos = glm::vec3(-516,584,-138);
	cam->dir = glm::vec3(1,0,0);
	cam->up = glm::vec3(0,1,0);
	cam->fix_up_vector = true;
	cam->near = 1;
	cam->far = 12500;
	cam->make_current();
	Camera::default_camera_movement_speed = 0.1;


#ifdef CG_KART
    namespace mk = mario_kart;
    mk::Scene track;
	track.load_model_from_file("render-data/models/cg-kart/MooMoo/MooMoo_fixed_and_normalmapped.obj");
	const std::string starting_lines_name = "gate_WhiteLine18__mm_StartGrad";  
	const mk::SceneObject* starting_line = track.find_object_by_name(starting_lines_name);

	mk::Kart kart(glm::vec3(0, 0, 1));
	kart.load_model_from_file("render-data/models/cg-kart/Kart/kart.obj");
	const glm::vec3 start_center = starting_line->mesh.bounds.center();
	const glm::vec3 initial_position = glm::vec3{start_center.x, starting_line->mesh.bounds.max.y - kart.bounds.min.y + 1, start_center.z};
	kart.rb.position = initial_position;
	kart.attach_camera(cam);
	mk::Physics physics(track);

	physics.gravity(true);
	for (unsigned int i = 0; i < track.scene_objects.size(); ++i) {
		physics.add_static_mesh(track.scene_objects[i].mesh);
	}
    physics.add_dynamic_mesh(&kart);
    std::vector<drawelement_ptr> *scene = &(track.draw_elements);
#else
	std::vector<drawelement_ptr> sponza = MeshLoader::load("render-data/models/sponza/sponza.fixed.obj");
    std::vector<drawelement_ptr> *scene = &sponza;
#endif

	shader_ptr shader_plain_color  = make_shader("a1", "shaders/default.vert", "shaders/default.frag");
	shader_ptr shader_material     = make_shader("a2", "shaders/material.vert", "shaders/material.frag");
	shader_ptr shader_lighting     = make_shader("a3", "shaders/shading.vert", "shaders/shading.frag");

	shader_ptr light_rep_shader = make_shader("light-rep", "shaders/light_rep.vert", "shaders/light_rep.frag");
	std::vector<drawelement_ptr> light_rep = MeshLoader::load("render-data/models/sphere.obj", false, [&](const material_ptr &) { return light_rep_shader; });
	

	TimerQuery input_timer("input");
	TimerQuery update_timer("update");
	TimerQueryGL render_timer("render");
	TimerQueryGL render_sm_timer("render shadowmap");

	glm::vec4 cols[100];
	for (int i = 0; i < 100; ++i)
		cols[i] = glm::vec4(glm::linearRand(0.0f,1.0f), glm::linearRand(0.0f,1.0f), glm::linearRand(0.0f,1.0f), 1.0f);

	while (Context::running() && game_is_running) {
		// input handling
		input_timer.start();
		Camera::default_input_handler(Context::frame_time());
		Camera::current()->update();
		static uint32_t counter = 0;
		if (counter++ % 100 == 0) Shader::reload();
		input_timer.end();

		// UI
		ImGui::Begin("Lighting");
		static const char *items[] = { "Plain Color",
		                               "Material Color",
									   "Phong Lighting",
		};
		enum { 
			PlainColor,
			MaterialColor,
			PhongLighting,
			N
		};
		static int active_mode = PlainColor;
		if (ImGui::BeginCombo("Assignment", items[active_mode])) {
			for (int i = 0; i < N; ++i) {
				if (ImGui::Selectable(items[i], i==active_mode))
					active_mode = i;
				if (active_mode == i)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		static glm::vec3 dirlight_dir = glm::vec3(0.25,-.93,-.25);
		static glm::vec3 dirlight_col = glm::vec3(1.0,0.97,0.8);
		static float     dirlight_scale = 1.2f;
		static glm::vec3 pointlight_pos = glm::vec3(75,500,500);
		static glm::vec3 pointlight_col = glm::vec3(1.0,0.58,0.16);
		static float     pointlight_scale = 1.5f;
		ImGui::Separator();
		if (ImGui::SliderFloat3("Directional Light", &dirlight_dir.x, -1, 1))
			if (dirlight_dir == glm::vec3(0,0,0))
				dirlight_dir = glm::vec3(0,-1,0);
			else
				dirlight_dir = glm::normalize(dirlight_dir);
		ImGui::ColorEdit3("Directional Light Color", &dirlight_col.x);
		ImGui::SliderFloat("Directional Light Intensity Scale", &dirlight_scale, 0, 10);
		ImGui::Separator();
		ImGui::SliderFloat3("Point Light Position", &pointlight_pos.x, -500, 500);
		ImGui::ColorEdit3("Point Light Color", &pointlight_col.x);
		ImGui::SliderFloat("Point Light Intensity Scale", &pointlight_scale, 0, 20);
		ImGui::End();

		// update
		update_timer.start();
        static float pi = M_PI;
		float dt = Context::frame_time() / 1000.0f; //milliseconds
#ifdef CG_KART
        if (kart.attached_camera) {
			    if (Context::key_pressed(GLFW_KEY_SPACE))
                    if (kart.rb.grounded) {
				        kart.rb.add_force(glm::vec3(0.0f, 250.0f, 0.0f));
                        kart.rb.grounded = false;
                    }
			    if (Context::key_pressed(GLFW_KEY_W))
			    	kart.rb.add_force(rotate(kart.rb.orientation, 1000.0f*dt*kart.initial_forward));
			    if (Context::key_pressed(GLFW_KEY_S))
			    	kart.rb.add_force(-rotate(kart.rb.orientation, 1000.0f*dt*kart.initial_forward));
			    if (Context::key_pressed(GLFW_KEY_A))
			    	kart.rb.orientation = rotate(kart.rb.orientation, dt, glm::vec3(0.0f, 1.0f, 0.0f));
			    if (Context::key_pressed(GLFW_KEY_D))
			    	kart.rb.orientation = rotate(kart.rb.orientation, -dt, glm::vec3(0.0f, 1.0f, 0.0f));
			    if (Context::key_pressed(GLFW_KEY_R))
			    	kart.rb.position = initial_position;
            }
			if (Context::key_pressed(GLFW_KEY_1)) {
                if (kart.attached_camera)
				    kart.detach_camera();
            }
            if (Context::key_pressed(GLFW_KEY_2)) {
                if (!kart.attached_camera)
                    kart.attach_camera(cam);
            }
			physics.update(dt);
            //Keep this after the physics update as the colliders and camera are updated here
			kart.update(dt);
            kart.update_attached_camera(dt);
#endif

		update_timer.end();


		// render
		render_timer.start();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (active_mode == PlainColor) {
			shader_plain_color->bind();
			shader_plain_color->uniform("model", glm::mat4(1));
			shader_plain_color->uniform("view", Camera::current()->view);
			shader_plain_color->uniform("proj", Camera::current()->proj);
			// TODO
			// Teilaufgabe 1
			for (auto &de : *scene) {
				de->material->bind(shader_plain_color);
				de->draw(glm::mat4(1));
				de->material->unbind();
			}
#ifdef CG_KART
            //The Kart has a different model matrix every frame, so we draw it seperately
            shader_plain_color->uniform("model", glm::mat4x4(kart.rb.transform_matrix));
            for (auto &de : kart.draw_elements) {
                de->material->bind(shader_plain_color);
                de->draw(glm::mat4x4(kart.rb.transform_matrix));
                de->material->unbind();
            }
#endif
			shader_plain_color->unbind();
		}
		else {
			for (auto &de : *scene) {
				shader_ptr shader;
				if (active_mode == MaterialColor)      shader = shader_material;
				else if (active_mode == PhongLighting) shader = shader_lighting;

				de->shader = shader;
				de->bind();
				shader->uniform("cam_pos", Camera::current()->pos);
				shader->uniform("pointlight_pos", pointlight_pos);
				shader->uniform("pointlight_col", glm::pow(pointlight_col, glm::vec3(2.2f)));
				shader->uniform("pointlight_scale", pointlight_scale);
				shader->uniform("dirlight_dir", dirlight_dir);
				shader->uniform("dirlight_col", glm::pow(dirlight_col, glm::vec3(2.2f)));
				shader->uniform("dirlight_scale", dirlight_scale);
				de->draw(glm::mat4(1));
				de->unbind();
			}
#ifdef CG_KART
			for (auto &de : kart.draw_elements) {
				shader_ptr shader;
				if (active_mode == MaterialColor)      shader = shader_material;
				else if (active_mode == PhongLighting) shader = shader_lighting;

				de->shader = shader;
				de->bind();
				shader->uniform("cam_pos", Camera::current()->pos);
				shader->uniform("pointlight_pos", pointlight_pos);
				shader->uniform("pointlight_col", glm::pow(pointlight_col, glm::vec3(2.2f)));
				shader->uniform("pointlight_scale", pointlight_scale);
				shader->uniform("dirlight_dir", dirlight_dir);
				shader->uniform("dirlight_col", glm::pow(dirlight_col, glm::vec3(2.2f)));
				shader->uniform("dirlight_scale", dirlight_scale);
				de->draw(glm::mat4x4(kart.rb.transform_matrix));
                de->unbind();
			}
#endif //CG_KART

		}
		render_timer.end();

		// finish frame
		Context::swap_buffers();
	}

	return 0;
}
