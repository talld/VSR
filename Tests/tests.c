#include <VSR.h>
#include "stdio.h"


int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480,
								   SDL_WINDOW_VULKAN);
	
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window,0L);
	VSR_Renderer* renderer = VSR_CreateRenderer(createInfo);
	
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_FreeRenderer(renderer);

	printf("exit successful");
	return 0;
}