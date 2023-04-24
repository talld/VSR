#ifndef VSR_SAMPLER_H
#define VSR_SAMPLER_H

#include <SDL2/SDL.h>

#include "VSR_Image.h"
#include "VSR_Framebuffer.h"

typedef struct VSR_Sampler VSR_Sampler;
struct VSR_Sampler
{
	uint64_t uuid;
	size_t arrayIndex;
	SDL_bool needsUpdate;

	uint32_t textureIndex;
	VSR_Image* image;
	VSR_ImageView* view;
	VkSampler sampler;

	VSR_Framebuffer* framebuffer;
};

VSR_Sampler*
VSR_SamplerCreate(
	VSR_Renderer* renderer,
	size_t textureIndex,
	SDL_Surface* sur,
	VSR_SamplerFlags flags);

void
VSR_SamplerFree(
	VSR_Renderer* renderer,
	VSR_Sampler* sampler);

VkSampler
VSR_GetTextureSampler(
	VSR_Renderer* renderer);

void
VSR_CreateTextureSampler(
	VSR_Renderer* renderer);

void VSR_SamplerWriteToDescriptor(
	VSR_Renderer* renderer,
	size_t index,
	VSR_Sampler* sampler);


#endif //VSR_SAMPLER_H