#ifndef VSR_SUITE_HELPERS_H
#define VSR_SUITE_HELPERS_H

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

inline static void
writeSPIRVToFile(const char* fNameIn, const char* fNameOut);

uint8_t* loadShader(const char* fName, size_t* n);

inline void loadFile(void *ctx, const char * filename, const int is_mtl, const char *obj_filename, char ** buffer, size_t * len);


typedef struct Mesh Mesh;
struct Mesh
{
	size_t numVertices;
	size_t numNormals;
	size_t numTexcoords;
	size_t numIndices;

	float* vertices;
	float* normals;
	float* texcoords;
	uint32_t* indices;
};

Mesh loadObj(const char * filename, const char * search_path);


#endif //VSR_SUITE_HELPERS_H
