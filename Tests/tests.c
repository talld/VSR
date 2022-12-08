#include <VSR.h>
#include <cglm/cglm.h>
#include "stdio.h"

void writeSPIRVToFile(const char* fNameIn, const char* fNameOut)
{
	////////////////////////////////////
	/// Dump spir-v shader to buffer ///
	////////////////////////////////////
	FILE* fIn = fopen(fNameIn, "r");
	fseek(fIn, 0, SEEK_END);
	size_t fInSize = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	uint8_t* byteCode = SDL_calloc(1, fInSize);
	fread(byteCode, fInSize, 1, fIn);
	fclose(fIn);

	////////////////////////////
	/// write buffer to file ///
	////////////////////////////
	FILE* fOut = fopen(fNameOut, "w");

	char buf[256];
	sprintf(buf, "#define kShaderBytecodeSize %lu\n", fInSize);
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

uint8_t* loadShader(const char* fName, size_t* n)
{
	////////////////////////////////////
	/// Dump spir-v shader to buffer ///
	////////////////////////////////////
	FILE* fIn = fopen(fName, "r");
	fseek(fIn, 0, SEEK_END);
	size_t fSize = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	uint8_t* byteCode = calloc(1, fSize);
	fread(byteCode, fSize, 1, fIn);
	fclose(fIn);

	*n = fSize;
	return byteCode;
}

int main(int argc, char* argv[])
{
///*
	writeSPIRVToFile("vert.spv", "vert.h");
	writeSPIRVToFile("frag.spv", "frag.h");
//*/
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480,
								   SDL_WINDOW_VULKAN);
	
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window,0L);
	VSR_Renderer* renderer = VSR_RendererCreate(createInfo);

	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);

	size_t n;
	uint8_t* vertBytes = loadShader("vert.spv", &n);
	VSR_Shader* vertShader = VSR_ShaderCreate(renderer, n, vertBytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_VERTEX, vertShader);
	free(vertBytes);

	uint8_t* fragBytes = loadShader("frag.spv", &n);
	VSR_Shader* fragShader = VSR_ShaderCreate(renderer, n, fragBytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_FRAGMENT, fragShader);
	free(fragBytes);

	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);

	VSR_RendererSetPipeline(renderer, pipeline);


	enum {kVertexCount= 8, kIndexCount = 6 * 6};
	VSR_Vertex verts[kVertexCount] = {
		//Top
		{-1, -1, -1}
		, {1, -1, -1}
		, {1, 1, -1}
		, {-1, 1, -1}
		, {-1, -1, 1}
		, {1, -1, 1}
		, {1, 1, 1,}
		, { -1, 1, 1 }
	};

	VSR_UV UVs[kVertexCount] = {
		//Top
		{0.5, 1}
		, {0.5, 1}
		, {0.5, 1}
		, {1, 1}
		, {1, 1}
		, {1, 1}
		, {1, 1}
		, { 1, 1}
	};

	VSR_Index indices[kIndexCount] =
			{
				{0}, {1}, {3},
				{3}, {1}, {2},
				{1}, {5}, {2},
				{2}, {5}, {6},
				{5}, {4}, {6},
				{6}, {4}, {7},
				{4}, {0}, {7},
				{7}, {0}, {3},
				{3}, {2}, {7},
				{7}, {2}, {6},
				{4}, {5}, {0},
				{0}, {5}, {1}
			};

	VSR_Mesh* mesh = VSR_MeshCreate(
		kVertexCount,
		verts,
		UVs,
		kIndexCount,
		indices);

	VSR_Model* mod = VSR_ModelCreate(renderer, mesh);

	mat4 model;
	glm_mat4_identity(model);

	mat4 view;

	vec3 eye = {0.f, 0.f, 6.f};
	vec3 center = {0.f, 0.f, 0.f};
	vec3 up = {0.f, 1.f, 0.f};

	glm_lookat(
		eye,
		center,
		up,
		view
		);

	float deg = 45.f;
	glm_make_rad(&deg);

	mat4 projection;
	glm_perspective(
		deg,
		640.f / 480.f,
		0.1f,
		100.f,
		projection);

	projection[1][1] *= -1;


	mat4 transform;
	glm_mat4_mul(projection, view, view);
	glm_mat4_mul(view, model, transform);

	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{
		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, mod, (VSR_Mat4*)&transform, 1);
		VSR_RendererEndPass(renderer);

		glm_rotate(transform, 0.01f, (vec3){0.0f, 1.f, 0.f});
		glm_rotate(transform, 0.01f, (vec3){1.0f, 0.f, 0.f});

		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			shouldQuit = 1;
		}
	}

	VSR_MeshFree(mesh);
	VSR_ModelFree(renderer, mod);

	VSR_ShaderDestroy(renderer, vertShader);
	VSR_ShaderDestroy(renderer, fragShader);

	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);

	printf("exit successful");
	return 0;
}