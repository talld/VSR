#include "VSR_Mesh.h"

#include <SDL2/SDL.h>

#include "VSR_error.h"





//==============================================================================
// VSR_MeshCreate
//------------------------------------------------------------------------------
VSR_Mesh*
VSR_MeshCreate(
	size_t vertexCount,
	VSR_Vertex const* vertices,
	VSR_UV const* UVs,
	size_t indexCount,
	VSR_Index const* indices)
{
	VSR_Mesh* mesh;
	mesh = SDL_malloc(sizeof(VSR_Mesh));

	size_t vertSize = vertexCount * sizeof(VSR_Vertex);
	mesh->vertices = malloc(vertSize);
	mesh->vertexCount = vertexCount;
	memcpy(mesh->vertices, vertices, vertSize);

	mesh->UVs = NULL;
	if(UVs)
	{
		size_t UVSize = vertexCount * sizeof(VSR_UV);
		mesh->UVs = malloc(UVSize);
		memcpy(mesh->UVs, UVs, UVSize);
	}

	mesh->indexCount = 0;
	mesh->indices = NULL;
	if(indices)
	{
		size_t indicesSize = indexCount * sizeof(VSR_Index);
		mesh->indices    = malloc(indicesSize);
		mesh->indexCount = indexCount;
		memcpy(mesh->indices, indices, indicesSize);
	}

	return mesh;
}





//==============================================================================
// VSR_MeshFree
//------------------------------------------------------------------------------
void
VSR_MeshFree(
	VSR_Mesh* mesh)
{
	SDL_free(mesh->vertices);
	SDL_free(mesh->UVs);

	if(mesh->indices)
	{
		SDL_free(mesh->indices);
	}

	SDL_free(mesh);
}