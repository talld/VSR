#ifndef VSR_MODEL_H
#define VSR_MODEL_H

VSR_Model*
VSR_ModelCreate(
	VSR_Renderer* renderer,
	float* vertices,
	size_t vertexCount,
	uint32_t* indices,
	size_t indexCount);

void
VSR_ModelFree(
	VSR_Renderer* renderer,
	VSR_Model* model);

int
VSR_BindUVs(
	VSR_Renderer* renderer,
	VSR_Model* model,
	float* UVs);

void VSR_ModelUpdate(VSR_Renderer* renderer, VSR_Model* model);

#endif //VSR_MODEL_H
