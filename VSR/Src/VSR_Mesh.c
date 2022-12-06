#include "VSR_Mesh.h"

#include <SDL2/SDL.h>

#include "VSR_error.h"





//==============================================================================
// VSR_MeshCreate
//------------------------------------------------------------------------------
VSR_Mesh*
VSR_MeshCreate(
	VSR_Vertex* vertices,
	size_t vertexCount,
	VSR_Index* indices,
	size_t indexCount)
{
	VSR_Mesh* mesh;
	mesh = SDL_malloc(sizeof(VSR_Mesh));

	size_t vertSize = vertexCount * sizeof(VSR_Vertex);
	mesh->vertices = malloc(vertSize);
	mesh->vertexCount = vertexCount;
	memcpy(mesh->vertices, vertices, vertSize);


	mesh->indices = indices;
	if(indices)
	{
		size_t indicesSize = indexCount * sizeof(VSR_Index);
		mesh->indices    = malloc(indicesSize);
		mesh->indexCount = indexCount;
		memcpy(mesh->indices, indices, indicesSize);
	}

	mesh->UVs = NULL;

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

	if(mesh->indices)
	{
		SDL_free(mesh->indices);
	}

	if(mesh->UVs)
	{
		SDL_free(mesh->UVs);
	}

	SDL_free(mesh);
}