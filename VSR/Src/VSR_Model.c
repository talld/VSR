#include "VSR_Model.h"

#include "VSR_Renderer.h"





//==============================================================================
// VSR_ModelCreate
//------------------------------------------------------------------------------
VSR_Model*
VSR_ModelCreate(
	VSR_Renderer* renderer,
	VSR_Mesh* mesh)
{
	VSR_Model* model = SDL_malloc(sizeof(VSR_Model));
	model->mesh = mesh;

	VSR_ModelUpdate(renderer, model);

	return model;
}





//==============================================================================
// VSR_ModelFree
//------------------------------------------------------------------------------
void
VSR_ModelFree(
	VSR_Renderer* renderer,
	VSR_Model* model)
{
	SDL_free(model);
}





//==============================================================================
// VSR_ModelUpdate
//------------------------------------------------------------------------------
void
VSR_ModelUpdate(
	VSR_Renderer* renderer,
	VSR_Model* model)
{
	size_t vertSize = model->mesh->vertexCount * sizeof(VSR_Vertex);

	Renderer_Memory* vertexStageMem = &renderer->subStructs->VUIStagingBuffer;
	Renderer_MemoryReset(vertexStageMem); // this should NEVER have state!

	Renderer_MemoryAlloc vertexStage = Renderer_MemoryAllocate(
			renderer,
			vertexStageMem,
			vertSize);

	void* p = Render_MemoryMapAlloc(renderer, *vertexStageMem, vertexStage);
	memcpy(p, model->mesh->vertices, vertSize);
	Render_MemoryUnmapAlloc(renderer, *vertexStageMem);


	Renderer_Memory* vertexGPUMem = &renderer->subStructs->VUIGPUBuffer;
	Renderer_MemoryAlloc vertGPU = Renderer_MemoryAllocate(
		 renderer,
		 vertexGPUMem,
		 vertSize);

	Renderer_MemoryTransfer(renderer,
							*vertexGPUMem,
							vertGPU.offset,
							*vertexStageMem,
							vertexStage.offset,
							vertGPU.size);

	model->vertices = vertGPU;
}