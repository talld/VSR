#include "VSR_Mat4.h"
#include <SDL2/SDL.h>


VSR_Mat4* VSR_Mat4Create(VSR_Renderer* renderer, float* m)
{
	static size_t uuid = 0;
	if(uuid == 0) {uuid++;}

	VSR_Mat4* mat4 = SDL_malloc(sizeof(VSR_Mat4));
	mat4->uuid = uuid++;
	mat4->arrayIndex = -1;
	mat4->needsUpdate = SDL_TRUE;

	SDL_memcpy(&mat4->m0, m, sizeof(float[16]));
}

void VSR_Mat4Destroy(VSR_Renderer* renderer, VSR_Mat4* mat4)
{

}