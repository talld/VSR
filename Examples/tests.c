#include <VSR.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <math.h>
#include "cube.h"
#include "helpers.h"

#include<Windows.h>

int SDL_main(int argc, char *argv[])
{
	// setup sdl
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480, SDL_WINDOW_VULKAN);

	// setup renderer
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window);
	VSR_Renderer* renderer = VSR_RendererCreate(createInfo);

	// create models
	VSR_Mesh* cubeMesh = VSR_MeshCreate(
		kCubeVertexCount,
		kCubeVertices,
		NULL,
		kCubeUVs,
		kCubeIndexCount,
		kCubeIndices
	);
	VSR_Model* cubeModel = VSR_ModelCreate(renderer, cubeMesh);
	mat4 cubePos;
	glm_mat4_identity(cubePos);
	// set images
	SDL_Surface* sur1 = SDL_LoadBMP("C:\\Users\\Ewain\\Dev\\22-23_CE301_williams_ewain\\Examples\\Assets\\castle_wall_albedo.bmp");
	VSR_Sampler* sampler1 = VSR_SamplerCreate(renderer, 1, sur1);

	// create and set pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);
	size_t n;
	uint8_t* bytes = loadShader("C:\\Users\\Ewain\\Dev\\22-23_CE301_williams_ewain\\Examples\\vert.spv", &n);
	VSR_Shader* vert = VSR_ShaderCreate(renderer, n, bytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_VERTEX, vert);
	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);
	VSR_RendererSetPipeline(renderer, pipeline);

	enum {kCubeRoot = 10};

	enum {kCubeCount = kCubeRoot * kCubeRoot * kCubeRoot};
	VSR_Mat4** cubePositions = SDL_malloc(sizeof(mat4*) * kCubeCount);;
	VSR_Sampler** cubeSamplers = SDL_malloc(sizeof(VSR_Sampler*) * kCubeCount);

	size_t breadth = kCubeRoot * kCubeRoot;
	size_t width = kCubeRoot;

	for (size_t k = 0; k < kCubeRoot; k++)
	{
		for (size_t j = 0; j < kCubeRoot; j++)
		{
			for (size_t i = 0; i < kCubeRoot; i++)
			{
				size_t index = i + (j * width) + (k * breadth);

				vec4 offset = {
					-125 + 2.5f * i,
					-150 + 2.5f * j,
					-2.5f * k
				};

				vec4 axis = {
					0.5f + (0.5f * i),
					0.5f + (0.5f * j),
					0.5f + (0.5f * k)
				};

				mat4 mat;
				glm_mat4_identity(mat);
				glm_translate(mat, offset);
				glm_rotate(mat, 0.1f, axis);
				VSR_Mat4* mat4 = VSR_Mat4Create(renderer, (float *) mat);
				cubePositions[index] = mat4;
				cubeSamplers[index] = sampler1;
			}
		}
	}

	mat4 view;
	vec3 eye = {0.f, 0.f, 250.f};
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
		400.f,
		projection);

	glm_mat4_mul(projection, view, view);

	VSR_PushConstants pushConstants;
	pushConstants.Projection = (VSR_Mat4*) &view;
	VSR_RendererSetVertexConstants(renderer, &pushConstants);
	VSR_RendererSetFragmentConstants(renderer, &pushConstants);

	// renderloop
	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{
		unsigned __int64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		double timerFrequency = (1.0 / freq);
		unsigned __int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, cubeModel, (VSR_Mat4**)cubePositions, cubeSamplers, kCubeCount);
		VSR_RendererEndPass(renderer);

		unsigned __int64 endTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
		double timeDifferenceInMilliseconds = ((endTime - startTime) * timerFrequency);
		printf("%f\n", timeDifferenceInMilliseconds);

		SDL_PollEvent(&event);
 		if(event.type == SDL_QUIT) {shouldQuit = 1;}

	}

	// cleanup
	VSR_SamplerFree(renderer, sampler1);
	VSR_MeshFree(cubeMesh);
	VSR_ModelFree(renderer, cubeModel);
	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);
	printf("exit successful");
	return 0;
}

#ifndef main
int main(void) 
{
	return SDL_main(0, NULL);
}
#endif