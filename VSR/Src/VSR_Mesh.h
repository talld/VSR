#ifndef VSR_MODEL_H
#define VSR_MODEL_H

#include <VSR.h>

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>


VSR_Mesh*
VSR_MeshCreate(
	size_t vertexCount,
	VSR_Vertex const* vertices,
	VSR_UV const* UVs,
	size_t indexCount,
	VSR_Index const* indices);

void
VSR_MeshFree(
	VSR_Mesh* model);

int
VSR_MeshBindUVs(
	VSR_Mesh* model,
	float* UVs);

#endif //VSR_MODEL_H
