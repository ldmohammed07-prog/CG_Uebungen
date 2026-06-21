#include "cmdline.h"

#include <argp.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

const char *argp_program_version = "1";

static char doc[]       = "renderpipeline: a simple software renderer.";
static char args_doc[]  = "";

// long option without corresponding short option have to define a symbolic constant >= 300
enum { FIRST = 300 };

static struct argp_option options[] = 
{
	// --[opt]		short/const		arg-descr		flag	option-descr
	{ "verbose", 			'v', 	0,         		0, "Be verbose." },
	{ "scene",              's',    "string",       0, "Which scene to use. Say -s list to get a list." },
	{ "width",              'w',    "int",          0, "View port width" },
	{ "height",             'h',    "int",          0, "View port height" },
	{ "near",               'n',    "float",        0, "Near plane distance (absolute value)" },
	{ "far",                'f',    "float",        0, "Far plane distance (absolute value)" },
	{ "tri-z",              'Z',    "float",        0, "z-value for {ndc,es}-tri" },
	{ "dir",                'd',    "vec3",         0, "view direction (default 0,0,-1)" },
	{ "up",                 'u',    "vec3",         0, "world up vector (default 0,1,0)" },
	{ "pos",                'p',    "vec3",         0, "camera position (default 0,0,0)" },
	{ "output-file",        'o',    "filename.png", 0, "output file name (default out.png)" },
	{ 0 }
};

string& replace_nl(string &s) {
	for (int i = 0; i < s.length(); ++i)
		if (s[i] == '\n' || s[i] == '\r')
			s[i] = ' ';
	return s;
}

glm::vec3 read_vec3(const std::string &s) {
	istringstream iss(s);
	glm::vec3 v;
	char sep;
	iss >> v.x >> sep >> v.y >> sep >> v.z;
	return v;
}


static error_t parse_options(int key, char *arg, argp_state *state) {
	// call argp_usage to stop program execution if something is wrong
	string sarg;
	if (arg)
		sarg = arg;
	sarg = replace_nl(sarg);

	switch (key)
	{
	case 'v':	cmdline.verbose = true; 	break;
	case 'w':	cmdline.vp_w = atoi(arg); 	break;
	case 'h':	cmdline.vp_h = atoi(arg); 	break;
	case 'n':	cmdline.n = atof(arg); 		break;
	case 'f':	cmdline.f = atof(arg); 		break;
	case 'Z':   cmdline.user_z = atof(arg);
				cmdline.user_z_defined = true;
				break;
	case 'd':   cmdline.view_dir = read_vec3(sarg); break;
	case 'u':   cmdline.world_up = read_vec3(sarg); break;
	case 'p':   cmdline.cam_pos  = read_vec3(sarg); break;
	case 'o':   cmdline.outfile = sarg; break;

	case 's':   if (sarg == "list") {
					cout << "ndc-tri:  a triangle in NDC (default)" << endl;
					cout << "es-tri:   a triangle in eye space" << endl;
					cout << "bunny:    a bunny model in world space" << endl;
				}
				else if (sarg == "ndc-tri") {
					cmdline.scene = ndc_tri;
					cmdline.pipeline = window_only;
				}
				else if (sarg == "es-tri") {
					cmdline.scene = es_tri;
					cmdline.pipeline = proj_window;
				}
				else if (sarg == "bunny") {
					cmdline.scene = bunny;
					cmdline.pipeline = view_proj_window;
				}
				else 
					argp_usage(state);
				break;

	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp parser = { options, parse_options, args_doc, doc };

int parse_cmdline(int argc, char **argv) {
	int ret = argp_parse(&parser, argc, argv, /*ARGP_NO_EXIT*/0, 0, 0);
	return ret;
}
	
struct cmdline cmdline;

