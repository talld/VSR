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
	////////////////////////
	/// update mesh info ///
	////////////////////////
	model->vertexCount = model->mesh->vertexCount;
	model->indexCount = model->mesh->indexCount;

	///////////////
	/// get mem ///
	///////////////
	Renderer_Memory* stageMem = &renderer->subStructs->VUVIStagingBuffer;
	Renderer_Memory* GPUMem = &renderer->subStructs->VUVIGPUBuffer;

	////////////////
	/// vertices ///
	////////////////
	size_t vertSize = model->mesh->vertexCount * sizeof(VSR_Vertex);

	Renderer_MemoryReset(stageMem); // this should NEVER have state!
	Renderer_MemoryAlloc stageV = Renderer_MemoryAllocate(
			renderer,
			stageMem,
			vertSize);

	void* pV = Render_MemoryMapAlloc(renderer, *stageMem, stageV);
	memcpy(pV, model->mesh->vertices, vertSize);
	Render_MemoryUnmapAlloc(renderer, *stageMem);


	Renderer_MemoryAlloc vertGPU = Renderer_MemoryAllocate(
		 renderer,
		 GPUMem,
		 vertSize);

	Renderer_MemoryTransfer(renderer,
							*GPUMem,
							vertGPU.offset,
							*stageMem,
							stageV.offset,
							vertGPU.size);

	model->vertices = vertGPU;

	////////////
	/// UVs ///
	///////////
	if(model->mesh->UVs)
	{
		size_t UVSize = model->mesh->vertexCount * sizeof(VSR_UV);

		Renderer_MemoryReset(stageMem); // this should NEVER have state!
		Renderer_MemoryAlloc stageUV = Renderer_MemoryAllocate(
			renderer,
			stageMem,
			UVSize);

		void* pUV = Render_MemoryMapAlloc(renderer, *stageMem, stageUV);
		memcpy(pUV, model->mesh->UVs, UVSize);
		Render_MemoryUnmapAlloc(renderer, *stageMem);


		Renderer_MemoryAlloc UVGPU = Renderer_MemoryAllocate(
			renderer,
			GPUMem,
			UVSize);

		Renderer_MemoryTransfer(renderer,
								*GPUMem,
								UVGPU.offset,
								*stageMem,
								stageUV.offset,
								UVGPU.size);

		model->UVs = UVGPU;
	}

	///////////////
	/// indices ///
	///////////////
	if(model->indexCount)
	{
		size_t indSize = model->mesh->indexCount * sizeof(VSR_Index);

		Renderer_MemoryReset(stageMem); // reset for indices use
		Renderer_MemoryAlloc stageI = Renderer_MemoryAllocate(
			renderer,
			stageMem,
			indSize);

		void* pI = Render_MemoryMapAlloc(renderer, *stageMem, stageI);
		memcpy(pI, model->mesh->indices, indSize);
		Render_MemoryUnmapAlloc(renderer, *stageMem);

		// create GPU index buffer
		Renderer_MemoryAlloc indGPU = Renderer_MemoryAllocate(
			renderer,
			GPUMem,
			indSize);

		Renderer_MemoryTransfer(renderer,
								*GPUMem,
								indGPU.offset,
								*stageMem,
								stageI.offset,
								indGPU.size);

		model->indices = indGPU;
	}

}