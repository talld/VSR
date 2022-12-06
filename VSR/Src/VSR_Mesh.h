#ifndef VSR_MODEL_H
#define VSR_MODEL_H

VSR_Mesh*
VSR_MeshCreate(
	VSR_Vertex* vertices,
	size_t vertexCount,
	VSR_Index* indices,
	size_t indexCount);

void
VSR_MeshFree(
	VSR_Mesh* model);

int
VSR_MeshBindUVs(
	VSR_Mesh* model,
	float* UVs);

#endif //VSR_MODEL_H
