#include <VSR.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <math.h>

#include "teapot.h"
#include "cube.h"

void writeSPIRVToFile(const char* fNameIn, const char* fNameOut)
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

uint8_t* loadShader(const char* fName, size_t* n)
{
	////////////////////////////////////
	/// Dump spir-v shader to buffer ///
	////////////////////////////////////
	FILE* fIn = fopen(fName, "rb");
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
	// setup sdl
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480, SDL_WINDOW_VULKAN);

	// setup renderer
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window,0L);
	VSR_Renderer* renderer = VSR_RendererCreate(createInfo);

	// config pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);

	// setup shaders
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

	// set pipeline
	VSR_RendererSetPipeline(renderer, pipeline);

	// create models
	VSR_Mesh* cubeMesh = VSR_MeshCreate(
		kCubeVertexCount,
		kCubeVertices,
		kCubeUVs,
		kCubeIndexCount,
		kCubeIndices
	);

	VSR_Mesh* teapotMesh = VSR_MeshCreate(
		kTeapotVertexCount,
		kTeapotVertices,
		kTeapotUVs,
		0,
		NULL
	);

	VSR_Model* cubeModel = VSR_ModelCreate(renderer, cubeMesh);

	enum {kTeaPotCount = 0};
	VSR_Model* teaModel[kTeaPotCount];

	for(int i = 0; i < kTeaPotCount; ++i)
	{
		teaModel[i] = VSR_ModelCreate(renderer, teapotMesh);
	}
	// set images
	SDL_Surface* sur = SDL_LoadBMP("wall.bmp");
	VSR_SamplerCreate(renderer, pipeline, 0, sur);

	// Maths for MVP
	mat4 cubePos;


	mat4 teaPos[kTeaPotCount];

	int root = cbrt((double)kTeaPotCount);


	for(int k = 0; k < root; ++k)
	{
		for(int j = 0; j < root; ++j)
		{
			for(int i = 0; i < root; ++i)
			{
				vec3 pos = {
					-0 + (float) 5 * i,
					-0 + (float) 5 * j,
					-0.1 //- (float) 5 * k
				};

				size_t index = i + (j * root) + (k * root * root);

				glm_mat4_identity(teaPos[index]);

				glm_translate(teaPos[index], pos);


			}
		}
	}

	glm_mat4_identity(cubePos);
	glm_translate(cubePos, (vec3){1.3f,0.f,-2.f});


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

	glm_mat4_mul(projection, view, view);

	// renderloop

	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{
		mat4 transform;

		VSR_RendererBeginPass(renderer);

		VSR_RenderModels(renderer, cubeModel, (VSR_Mat4*)&transform, 1);

		for(int i = 0; i < kTeaPotCount; i++)
		{
			glm_mat4_mul(view, teaPos[i], transform);
			VSR_RenderModels(renderer, teaModel[i], (VSR_Mat4*) &transform, 1);
		}

		VSR_RendererEndPass(renderer);

		for(int i = 0; i < kTeaPotCount; i++)
		{
			glm_rotate(teaPos[i], 0.01f, (vec3) {0.0f, 1.f, 0.f});
		}

		glm_mat4_mul(view, cubePos, transform);
		glm_rotate(cubePos, 0.01f, (vec3){0.0f, 1.f, 0.f});
		glm_rotate(cubePos, 0.01f, (vec3){1.0f, 0.f, 0.f});



		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			shouldQuit = 1;
		}
	}

	// cleanup
	VSR_MeshFree(cubeMesh);
	VSR_MeshFree(teapotMesh);
	VSR_ModelFree(renderer, cubeModel);

	for(int i = 0; i < kTeaPotCount; i++)
	{
		VSR_ModelFree(renderer, teaModel[i]);
	}

	VSR_ShaderDestroy(renderer, vertShader);
	VSR_ShaderDestroy(renderer, fragShader);

	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);

	printf("exit successful");
	return 0;
}