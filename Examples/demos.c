#include "LightingDemo/lightingDemo.h"
#include "CubeDemo/cubeDemo.h"

int
SDL_main(int argc, char** argv)
{
	lightingDemo(argc, argv);
	//cubeDemo(argc, argv);
	return 0;
}

#ifndef main
int
main(int argc, char** argv)
{
	return SDL_main(argc, argv);
}
#endif