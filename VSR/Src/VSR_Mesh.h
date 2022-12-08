#ifndef VSR_MODEL_H
#define VSR_MODEL_H

VSR_Mesh*
VSR_MeshCreate(
	size_t vertexCount,
	VSR_Vertex* vertices,
	VSR_UV* UVs,
	size_t indexCount,
	VSR_Index* indices);

void
VSR_MeshFree(
	VSR_Mesh* model);

int
VSR_MeshBindUVs(
	VSR_Mesh* model,
	float* UVs);

#endif //VSR_MODEL_H
