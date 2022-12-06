#ifndef VSR_MODEL_H
#define VSR_MODEL_H

#include "VSR_Mesh.h"
#include "Renderer_Memory.h"

typedef struct VSR_Model VSR_Model;
struct VSR_Model
{
	VSR_Mesh* mesh; // callback for counts
	size_t vertexCount;
	Renderer_MemoryAlloc vertices;
	Renderer_MemoryAlloc UVs;
	size_t indexCount;
	Renderer_MemoryAlloc indices;
};

VSR_Model*
VSR_ModelCreate(
	VSR_Renderer* renderer,
	VSR_Mesh* mesh);

void
VSR_ModelFree(
	VSR_Renderer* renderer,
	VSR_Model* model);

void
VSR_ModelUpdate(
	VSR_Renderer* renderer,
	VSR_Model* model);


#endif //VSR_MODEL_H
