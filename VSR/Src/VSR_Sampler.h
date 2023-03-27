#ifndef VSR_SAMPLER_H
#define VSR_SAMPLER_H

#include "SDL2/SDL.h"

#include "VSR_Image.h"

typedef struct VSR_Sampler VSR_Sampler;
struct VSR_Sampler
{
	size_t textureIndex;
	VSR_Image* image;
	VSR_ImageView* view;
	VkSampler sampler;
};

VSR_Sampler*
VSR_SamplerCreate(
	VSR_Renderer* renderer,
	size_t textureIndex,
	SDL_Surface* sur);

void
VSR_SamplerFree(
	VSR_Renderer* renderer,
	VSR_Sampler* sampler);

VkSampler
VSR_GetTextureSampler(
	VSR_Renderer* renderer);

void VSR_SamplerWriteToDescriptor(
	VSR_Renderer* renderer,
	size_t index,
	VSR_Sampler* sampler);


#endif //VSR_SAMPLER_H