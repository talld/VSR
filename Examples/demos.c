#include "CubeDemo/cubedemo.h"

int
SDL_main(int argc, char** argv)
{
	cubeDemo(argc, argv);
	return 0;
}

#ifndef main
int
main(int argc, char** argv)
{
	return SDL_main(argc, argv);
}
#endif