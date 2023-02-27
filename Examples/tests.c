#include <VSR.h>
#include <cglm/cglm.h>
#include "stdio.h"

#include "teapot.h"
#include "cube.h"

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
	// setup sdl
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480, SDL_WINDOW_VULKAN);

	// setup renderer
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window,0L);
	VSR_Renderer* renderer = VSR_RendererCreate(createInfo);

	// config pipeline
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

	// set pipeline
	VSR_RendererSetPipeline(renderer, pipeline);

	// create models
	VSR_Mesh* cubeMesh = VSR_MeshCreate(
		kCubeVertexCount,
		cubeVerts,
		cubeUVs,
		kCubeIndexCount,
		cubeIndices	);

	VSR_Mesh* teapotMesh = VSR_MeshCreate(
		kTeapotVertexCount,
		(VSR_Vertex*)teapotVerts,
		(VSR_UV*)teapotVerts,
		0,
		NULL);

	VSR_Model* cubeModel = VSR_ModelCreate(renderer, cubeMesh);
	VSR_Model* teaModel = VSR_ModelCreate(renderer, teapotMesh);

	// Maths for MVP
	mat4 cubePos;
	mat4 teaPos;
	glm_mat4_identity(cubePos);
	glm_mat4_identity(teaPos);
	glm_translate(cubePos, (vec3){-1.5f,0.f,0.f});
	glm_translate(teaPos, (vec3){1.5f,0.f,0.f});


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

		glm_mat4_mul(view, cubePos, transform);
		VSR_RenderModels(renderer, cubeModel, (VSR_Mat4*)&transform, 1);

		glm_mat4_mul(view, teaPos, transform);
		VSR_RenderModels(renderer, teaModel, (VSR_Mat4*)&transform, 1);

		VSR_RendererEndPass(renderer);

		glm_rotate(cubePos, 0.01f, (vec3){0.0f, 1.f, 0.f});
		glm_rotate(cubePos, 0.01f, (vec3){1.0f, 0.f, 0.f});

		glm_rotate(teaPos, 0.01f, (vec3){0.0f, 1.f, 0.f});

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
	VSR_ModelFree(renderer, teaModel);

	VSR_ShaderDestroy(renderer, vertShader);
	VSR_ShaderDestroy(renderer, fragShader);

	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);

	printf("exit successful");
	return 0;
}