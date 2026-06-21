#pragma once

#include <string>
#include <glm/glm.hpp>

enum scenes { ndc_tri, es_tri, bunny };
enum pipeline_config { window_only, proj_window, view_proj_window };

struct cmdline {
	bool verbose = false;
	int vp_w = 128,
		vp_h = 128;
	float n = 1;
	float f = 100;
	float fovy = 65;
	float user_z = 0;
	float user_z_defined = false;
	glm::vec3 view_dir = glm::vec3(0,0,-1);
	glm::vec3 world_up = glm::vec3(0,1,0);
	glm::vec3 cam_pos  = glm::vec3(0,0,0);
	std::string outfile = "out.png";
	scenes scene = ndc_tri;
	pipeline_config pipeline = window_only;
};

extern struct cmdline cmdline;

int parse_cmdline(int argc, char **argv);
