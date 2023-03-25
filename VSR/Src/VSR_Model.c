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

	model->vertices = NULL;
	model->normals = NULL;
	model->UVs = NULL;
	model->indices = NULL;

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
	Renderer_MemoryAllocFree(renderer, model->vertices);
	Renderer_MemoryAllocFree(renderer, model->UVs);
	Renderer_MemoryAllocFree(renderer, model->indices);
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

	if(!model->vertexCount) return;
	if(!model->indexCount) return;

	///////////////
	/// get mem ///
	///////////////
	Renderer_Memory* stageMem = renderer->VIStagingBuffer;
	Renderer_Memory* GPUMem = renderer->VIGPUBuffer;

	////////////////
	/// vertices ///
	////////////////
	size_t vertSize = model->mesh->vertexCount * sizeof(VSR_Vertex);

	Renderer_MemoryReset(stageMem); // this should NEVER have state!
	Renderer_MemoryAlloc *stageV = Renderer_MemoryAllocate(
		renderer,
		stageMem,
		vertSize,
		0);

	void *pV = Renderer_MemoryAllocMap(renderer, stageV);
	SDL_memcpy(pV, model->mesh->vertices, vertSize);
	Renderer_MemoryAllocUnmap(renderer, stageV);

	Renderer_MemoryAlloc *vertGPU = Renderer_MemoryAllocate(
		renderer,
		GPUMem,
		vertSize,
		0
	);

	Renderer_MemoryTransferAlloc(
		renderer,
		vertGPU,
		stageV
	);

	Renderer_MemoryAllocFree(renderer, stageV);
	model->vertices = vertGPU;

	///////////////
	/// normals ///
	///////////////
	size_t normalsSize = model->mesh->vertexCount * sizeof(VSR_Vertex);

	Renderer_MemoryReset(stageMem); // this should NEVER have state!
	Renderer_MemoryAlloc* stageN = Renderer_MemoryAllocate(
		renderer,
		stageMem,
		normalsSize,
		0);

	void *pN = Renderer_MemoryAllocMap(renderer, stageN);
	SDL_memcpy(pN, model->mesh->normals, normalsSize);
	Renderer_MemoryAllocUnmap(renderer, stageN);

	Renderer_MemoryAlloc* normalGPU = Renderer_MemoryAllocate(
		renderer,
		GPUMem,
		vertSize,
		0
	);

	Renderer_MemoryTransferAlloc(
		renderer,
		normalGPU,
		stageN
	);

	Renderer_MemoryAllocFree(renderer, stageV);
	model->normals = normalGPU;

	////////////
	/// UVs ///
	///////////
	size_t UVSize = model->mesh->vertexCount * sizeof(VSR_UV);

	Renderer_MemoryReset(stageMem); // this should NEVER have state!
	Renderer_MemoryAlloc *stageUV = Renderer_MemoryAllocate(
		renderer,
		stageMem,
		UVSize,
		0
	);
	void *pUV = Renderer_MemoryAllocMap(renderer, stageUV);
	SDL_memcpy(pUV, model->mesh->UVs, UVSize);
	Renderer_MemoryAllocUnmap(renderer, stageUV);


	Renderer_MemoryAlloc *UVGPU = Renderer_MemoryAllocate(
		renderer,
		GPUMem,
		UVSize,
		0
	);

	Renderer_MemoryTransferAlloc(
		renderer,
		UVGPU,
		stageUV
	);
	Renderer_MemoryAllocFree(renderer, stageUV);
	model->UVs = UVGPU;


	///////////////
	/// indices ///
	///////////////

	size_t indSize = model->mesh->indexCount * sizeof(VSR_Index);

	Renderer_MemoryReset(stageMem); // reset for indices use
	Renderer_MemoryAlloc *stageI = Renderer_MemoryAllocate(
		renderer,
		stageMem,
		indSize,
		0);

	void *pI = Renderer_MemoryAllocMap(renderer, stageI);
	memcpy(pI, model->mesh->indices, indSize);
	Renderer_MemoryAllocUnmap(renderer, stageI);

	// create GPU index buffer
	Renderer_MemoryAlloc *indGPU = Renderer_MemoryAllocate(
		renderer,
		GPUMem,
		indSize,
		0
	);

	Renderer_MemoryTransferAlloc(
		renderer,
		indGPU,
		stageI
	);

	Renderer_MemoryAllocFree(renderer, stageI);
	model->indices = indGPU;

}
