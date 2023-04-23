#ifndef VSR_SUITE_HELPERS_H
#define VSR_SUITE_HELPERS_H

#include <SDL2/SDL.h>

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#define TINYOBJ_MALLOC SDL_malloc
#define TINYOBJ_REALLOC SDL_realloc
#define TINYOBJ_CALLOC SDL_calloc
#define TINYOBJ_FREE SDL_free

/*
#include <tinyobj_loader_c.h>
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

inline static void
writeSPIRVToFile(const char* fNameIn, const char* fNameOut)
{
	////////////////////////////////////
	/// Dump spir-v shader to buffer ///
	////////////////////////////////////
	FILE* fIn = fopen(fNameIn, "rb");
	fseek(fIn, 0, SEEK_END);
	size_t fInSize = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	uint8_t* byteCode = SDL_calloc(1, fInSize);
	fread(byteCode, fInSize, 1, fIn);
	fclose(fIn);

	////////////////////////////
	/// write buffer to file ///
	////////////////////////////
	FILE* fOut = fopen(fNameOut, "wb");

	char buf[256];
	sprintf(buf, "#define kShaderBytecodeSize %lu\n", (long unsigned int)fInSize);
	fputs(buf, fOut);
	fputs("const uint8_t kShaderByteCode[kShaderBytecodeSize] = {\n", fOut);

	size_t bytesPerLine = 8;


	for(size_t i = 1; i <= fInSize; i++)
	{
		sprintf(buf, "0x%02x,", byteCode[i-1]);
		fputs(buf, fOut);

		if(i % bytesPerLine == 0)
		{
			fputs("\n", fOut);
		}
	}

	fputs("};\n", fOut);
	fclose(fOut);

	SDL_free(byteCode);
}

inline static uint8_t*
loadShader(const char* fName, size_t* n)
{
	////////////////////////////////////
	/// Dump spir-v shader to buffer ///
	////////////////////////////////////
	FILE* fIn = fopen(fName, "rb");
	fseek(fIn, 0, SEEK_END);
	size_t fSize = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	uint8_t* byteCode = calloc(1, fSize);
	if (byteCode != NULL)
	{
		fread(byteCode, fSize, 1, fIn);
		fclose(fIn);
		*n = fSize;
	}

	return byteCode;
}

inline void
loadFile(void *ctx, const char * filename, const int is_mtl, const char *obj_filename, char ** buffer, size_t * len)
{
	size_t string_size = 0;
	size_t read_size = 0;
	FILE *handle = fopen(filename, "rb");

	if (handle)
	{
		fseek(handle, 0, SEEK_END);
		string_size = ftell(handle);
		rewind(handle);

		*buffer = (char *) SDL_malloc(sizeof(char) * (string_size + 1));

		read_size = fread(*buffer, sizeof(char), string_size, handle);
		(*buffer)[string_size] = '\0';

		while (string_size != read_size)
		{
			size_t remaining = string_size - read_size;
			size_t remainingRead = fread(
				*buffer + read_size,
				sizeof(char),
				remaining,
				handle);

			read_size += remainingRead;
		}

		fclose(handle);
	}

	*len = read_size;
}

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

/*
static inline Mesh loadObj(const char * filename, const char * search_path)
{
	tinyobj_shape_t * shape = NULL;
	tinyobj_material_t * material = NULL;
	tinyobj_attrib_t attrib;
	tinyobj_attrib_init(&attrib);
	size_t num_shapes = 0;
	size_t num_materials = 0;

	int result = tinyobj_parse_obj(
		&attrib,
		&shape,
		&num_shapes,
		&material,
		&num_materials,
		filename,
		loadFile,
		NULL,
		TINYOBJ_FLAG_TRIANGULATE
	);

	Mesh mesh = (Mesh){0};


	if(result == TINYOBJ_SUCCESS)
	{
		size_t verticesSize = attrib.num_vertices * sizeof(float[3]);
		mesh.numVertices = attrib.num_vertices * 3;
		mesh.vertices = SDL_calloc(1, verticesSize);

		size_t normalsSize = attrib.num_normals * sizeof(float[3]);
		mesh.numNormals = attrib.num_normals;
		mesh.normals = SDL_calloc(1, normalsSize);

		size_t texcoordsSize = attrib.num_texcoords * sizeof(float[2]);
		mesh.numTexcoords = attrib.num_texcoords;
		mesh.texcoords = SDL_calloc(1, texcoordsSize);

		memcpy(mesh.vertices, attrib.vertices, verticesSize);
		memcpy(mesh.normals, attrib.normals, normalsSize);
		memcpy(mesh.texcoords, attrib.texcoords, texcoordsSize);

		size_t indicesSize = attrib.num_faces * sizeof(int32_t);
		mesh.numIndices = attrib.num_faces;
		mesh.indices = SDL_calloc(1, indicesSize);



		for( int i = 0; i < attrib.num_faces; i++)
		{
			if(attrib.face_num_verts[i / 3] != 3)
			{
				int faceNumVerts = attrib.face_num_verts[i];
				*(int*)NULL = faceNumVerts;
			}

			size_t vertexIndex = attrib.faces[i].v_idx;
			mesh.indices[i] = vertexIndex;
		}
	}

	tinyobj_attrib_free(&attrib);
	return mesh;
}
*/
#endif //VSR_SUITE_HELPERS_H
