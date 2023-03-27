#ifndef VSR_SUITE_VSR_MAT4_H
#define VSR_SUITE_VSR_MAT4_H

#include <VSR.h>

VSR_PACKED(struct VSR_Mat4)
{
	float m0;
 	float m1;
	float m2;
	float m3;
	float m4;
	float m5;
	float m6;
	float m7;
	float m8;
	float m9;
	float m10;
	float m11;
	float m12;
	float m13;
	float m14;
	float m15;

	uint64_t uuid;
	size_t arrayIndex;
	SDL_bool needsUpdate;
};

VSR_Mat4* VSR_Mat4Create(VSR_Renderer* renderer, float* m);

void VSR_Mat4Destroy(VSR_Renderer* renderer, VSR_Mat4* mat4);

#endif //VSR_SUITE_VSR_MAT4_H
