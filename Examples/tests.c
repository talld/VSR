#include <VSR.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <math.h>
#include "helpers.h"
#include "teapot.h"
#include "cube.h"
int main(int argc, char* argv[])
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
	SDL_Surface* sur1 = SDL_LoadBMP("Assets/castle_wall_albedo.bmp");
	VSR_Sampler* sampler1 = VSR_SamplerCreate(renderer, 1, sur1);

	// create and set pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);
	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);
	VSR_RendererSetPipeline(renderer, pipeline);

	enum {kCubeRoot = 100};

	enum {kCubeCount = kCubeRoot * kCubeRoot * kCubeRoot};
	mat4* cubePositions = SDL_malloc(sizeof(mat4) * kCubeCount);;
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
				cubeSamplers[index] = sampler1;

				vec4 offset = {
					-125 + 2.5f * i,
					-150 + 2.5f * j,
					-2.5f * k
				};

				glm_mat4_identity(cubePositions[index]);
				glm_translate(cubePositions[index], offset);
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
	pushConstants.Projection = *(VSR_Mat4*) view;
	VSR_RendererSetVertexConstants(renderer, &pushConstants);
	VSR_RendererSetFragmentConstants(renderer, &pushConstants);


	// renderloop
	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{

		for (size_t k = 0; k < kCubeRoot; k++)
		{
			for (size_t j = 0; j < kCubeRoot; j++)
			{
				for (size_t i = 0; i < kCubeRoot; i++)
				{
					size_t index = i + (j * width) + (k * breadth);


					vec4 axis = {
						0.5f + (0.5f * i),
						0.5f + (0.5f * j),
						0.5f + (0.5f * k)
					};

					glm_rotate(cubePositions[index], 0.1 , axis);
					cubeSamplers[index] = sampler1;
				}
			}
		}

		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, cubeModel, (VSR_Mat4*)cubePositions, cubeSamplers, kCubeCount);
		VSR_RendererEndPass(renderer);
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