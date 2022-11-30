#include "VSR_Model.h"

#include <SDL2/SDL.h>

#include "VSR_Renderer.h"
#include "VSR_error.h"

VSR_Model*
VSR_ModelCreate(
	VSR_Renderer* renderer,
	float* vertices,
	size_t vertexCount,
	uint32_t* indices,
	size_t indexCount)
{
	if((vertexCount % 3) != 0)
	{
		VSR_SetErr("Invalid vertex count provided for model (must % 3 == 0)");
		return NULL;
	}


	VSR_Model* model = SDL_malloc(sizeof(VSR_Model));

	size_t pointsSize = sizeof(float) * (vertexCount * 3);
	size_t vertsSize = sizeof(VSR_Vertex) * (vertexCount);

	if(pointsSize != vertsSize)
	{
		printf("error!");
		*(int*)NULL = 0;
	}

	model->vertexCount = vertexCount;
	model->vertices = SDL_malloc(vertsSize);
	memcpy(model->vertices, vertices, vertsSize);

	// optional values default to NULL
	model->UVs = NULL;
	model->indices = NULL;

	if(indices)
	{
		size_t indicesSize = sizeof(VSR_Index) * indexCount;
		model->indexCount = 0;
		model->indices    = SDL_malloc(indicesSize);
		memcpy(model->indices, indices, indicesSize);
	}

	VSR_ModelUpdate(renderer, model);
	return model;
}

void
VSR_ModelFree(
	VSR_Renderer* renderer,
	VSR_Model* model)
{
	SDL_free(model->vertices);

	if(model->indices)
	{
		SDL_free(model->indices);
	}

	if(model->UVs)
	{
		SDL_free(model->UVs);
	}
}

int
VSR_BindUVs(
	VSR_Renderer* renderer,
	VSR_Model* model,
	float* UVs)
{
	size_t UVsSize = sizeof(VSR_UV) * model->vertexCount;
	model->UVs = SDL_malloc(UVsSize);
	memcpy(model->UVs, UVs, UVsSize);
	return 0;
}





void VSR_ModelUpdate(VSR_Renderer* renderer, VSR_Model* model)
{
	///////////////////////////////////////////////////
	/// update and create (if needed) model buffers ///
	///////////////////////////////////////////////////

	// these can change at runtime due to the renderer moving stuff around
	model->vertexBufferIndex = Renderer_GetModelBufferIndex(renderer, model);
	size_t vertexSize = model->vertexCount * sizeof(*model->vertices);

	if(model->vertexBufferIndex == -1) // make sure we HAVE a vertex buffer
	tryAgain:
	{
		// find the nearest free entry

		size_t index = Renderer_GetModelBufferIndex(renderer, NULL);
		if(index == -1)
		{
			Renderer_ModelBuffer buf = Renderer_AllocateModelBuffer(
				renderer,
				model,
				RESOURCE_TYPE_VERTEX,
				vertexSize);

			index = Renderer_AppendModelBuffer(renderer, buf);
			model->vertexBufferIndex = index;
		}
	}

	void* p = Renderer_MapModelBuffer(renderer, model->vertexBufferIndex);
/*
	float* fp = (float*)p;
	printf(
		   "[0]: %f "
		   "[1]: %f "
		   "[2]: %f "
		   "[3]: %f "
		   "[4]: %f "
		   "[5]: %f "
		   "[6]: %f "
		   "[7]: %f "
		   "[8]: %f\n",
		   fp[0],
		   fp[1],
		   fp[2],
		   fp[3],
		   fp[4],
		   fp[5],
		   fp[6],
		   fp[7],
		   fp[8]);
*/
	memcpy(p, model->vertices, vertexSize);
	Renderer_UnmapModelBuffer(renderer, p);

}