#include <iostream>
#include <fstream>
#include "rtcore/objmesh.hpp"
#include "visualize.hpp"
#include "mesh_test.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "sphere_set_approximate.hpp"
#include "lib/argparse.h"

static const char *const usage[] = {
    "main -i <obj> -m <manifold> -n <n_sphere> [options]",
    NULL,
};

int main(int argc, const char* argv[])
{
	int n_sphere = 64;
	int n_innersample = 10000;
	int n_surfacesample = 5000;
	int n_finalsample = 200000;
	int n_mutate = 10;
	int seed = 19260817;
	const char *objpath = NULL;
	const char *manifoldpath = NULL;

	argparse_option opt[] = {
        OPT_GROUP("Options:"),
        OPT_HELP(),
        OPT_STRING('i', "obj", &objpath, "input mesh to approximate"),
        OPT_STRING('m', "manifold", &manifoldpath, "simplified manifold of input mesh"),
        OPT_INTEGER('n', "sphere", &n_sphere, "number of spheres to construct, default=64"),
        OPT_INTEGER(0, "inner", &n_innersample, "number of inner sample points, default=10000"),
        OPT_INTEGER(0, "surface", &n_surfacesample, "number of surface sample points, default=5000"),
        OPT_INTEGER(0, "final", &n_finalsample, "number of final samples, default=200000"),
        OPT_INTEGER(0, "mutate", &n_mutate, "number of attempts to jump out of local minima, default=10"),
        OPT_INTEGER(0, "seed", &seed, "seed of random number generator"),
        OPT_END(),
    };
    argparse parser;
    argparse_init(&parser, opt, usage, 0);
	argc = argparse_parse(&parser, argc, argv);
	if (!objpath || !manifoldpath) {
		argparse_usage(&parser);
		return 1;
	}
	visualizer_mesh_filename = objpath;
	RTcore::Mesh mesh = RTcore::objmesh(objpath);
	RTcore::Mesh manifold = RTcore::objmesh(manifoldpath);

	test_all_normal_outward(manifold);

	srand(seed);
	auto spheres = sphere_set_approximate(mesh, manifold, n_sphere, n_innersample, n_surfacesample, n_finalsample, n_mutate);

	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;
	visualize(spheres);
	console.log("Evaluating...");
	console.info("Relative Outside Volume (Er):", volume(spheres)/volume(manifold)-1);
}
