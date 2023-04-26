#include "CubeDemo/cubedemo.h"
#include "LightingDemo/lightingDemo.h"
#include "ShadowDemo/shadowDemo.h"

int
SDL_main(int argc, char** argv)
{
	//cubeDemo(argc, argv);
	//lightingDemo(argc, argv);
	shadowDemo(argc, argv);
	return 0;
}

#ifndef main
int
main(int argc, char** argv)
{
	return SDL_main(argc, argv);
}
#endif