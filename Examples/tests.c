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

	// create and set pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);
	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);
	VSR_RendererSetPipeline(renderer, pipeline);

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
	VSR_Sampler* sampler1 = VSR_SamplerCreate(renderer, pipeline, 1, sur1);
	// renderloop
	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{
		glm_rotate(cubePos, 0.1f, (vec3){0,1,0});
		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, cubeModel, (VSR_Mat4*)&cubePos, &sampler1, 1);
		VSR_RendererEndPass(renderer);
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT) {shouldQuit = 1;}
	}

	// cleanup
	VSR_MeshFree(cubeMesh);
	VSR_ModelFree(renderer, cubeModel);
	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);
	printf("exit successful");
	return 0;
}