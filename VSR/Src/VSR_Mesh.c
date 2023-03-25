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
	VSR_Vertex const* normals,
	VSR_UV const* UVs,
	size_t indexCount,
	VSR_Index const* indices)
{
	VSR_Mesh* mesh;
	mesh = SDL_malloc(sizeof(VSR_Mesh));

	////////////////
	/// vertices ///
	////////////////
	size_t vertSize = vertexCount * sizeof(VSR_Vertex);
	mesh->vertices = SDL_malloc(vertSize);
	mesh->vertexCount = vertexCount;
	memcpy(mesh->vertices, vertices, vertSize);

	///////////////
	/// normals ///
	///////////////
	mesh->normals = NULL;
	size_t normalSize = vertexCount * sizeof(VSR_Vertex);
	mesh->normals = SDL_malloc(normalSize);
	if(normals)
	{
		SDL_memcpy(mesh->normals, normals, normalSize);
	}
	else
	{
		SDL_memset(mesh->normals, 0, normalSize);
	}

	///////////
	/// UVs ///
	///////////
	mesh->UVs = NULL;
	size_t UVSize = vertexCount * sizeof(VSR_UV);
	mesh->UVs = SDL_malloc(UVSize);
	if(UVs)
	{
		SDL_memcpy(mesh->UVs, UVs, UVSize);
	}
	else
	{
		SDL_memset(mesh->UVs, 0, UVSize);
	}

	///////////////
	/// indices ///
	///////////////
	mesh->indexCount = 0;
	mesh->indices = NULL;
	if(indexCount)
	{
		size_t indicesSize = indexCount * sizeof(VSR_Index);
		mesh->indexCount   = indexCount;
		mesh->indices      = SDL_malloc(indicesSize);
		SDL_memcpy(mesh->indices, indices, indicesSize);
	}
	else
	{
		size_t indicesSize = vertexCount * sizeof(VSR_Index);
		mesh->indexCount   = vertexCount;
		mesh->indices      = SDL_malloc(indicesSize);
		for(int i = 0; i < vertexCount; i++)
		{
			mesh->indices[i] = *(VSR_Index*)&i;
		}
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
	SDL_free(mesh->indices);


	SDL_free(mesh);
}