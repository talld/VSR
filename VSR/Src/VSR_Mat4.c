#include "VSR_Mat4.h"


VSR_Mat4* VSR_Mat4Create(VSR_Renderer* renderer, float* m)
{
	static size_t uuid = 0;
	if(uuid == 0) {uuid++;}

	VSR_Mat4* mat4 = SDL_malloc(sizeof(VSR_Mat4));
	mat4->uuid = uuid++;
	mat4->arrayIndex = -1;
	mat4->needsUpdate = SDL_TRUE;

	mat4->m0 = 0;
	mat4->m1 = 0;
	mat4->m2 = 0;
	mat4->m3 = 0;
	mat4->m4 = 0;
	mat4->m5 = 0;
	mat4->m6 = 0;
	mat4->m7 = 0;
	mat4->m8 = 0;
	mat4->m9 = 0;
	mat4->m10 = 0;
	mat4->m11 = 0;
	mat4->m12 = 0;
	mat4->m13 = 0;
	mat4->m14 = 0;
	mat4->m15 = 0;
}

void VSR_Mat4Destroy(VSR_Renderer* renderer, VSR_Mat4* mat4)
{

}