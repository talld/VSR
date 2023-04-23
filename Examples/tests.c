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
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,512,512, SDL_WINDOW_VULKAN);

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
	VSR_Sampler* sampler1 = VSR_SamplerCreate(renderer, 1, sur1, 0);
	VSR_Sampler* sampler2 = VSR_SamplerCreate(renderer, 2, sur1, SAMPLER_FLAG_RENDER_TARGET);

	// create and set pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);
	size_t n;
	uint8_t* bytes = loadShader("C:\\Users\\Ewain\\Dev\\22-23_CE301_williams_ewain\\Examples\\vert.spv", &n);
	VSR_Shader* vert = VSR_ShaderCreate(renderer, n, bytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_VERTEX, vert);
	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);
	VSR_RendererSetPipeline(renderer, pipeline);

	mat4 mat;
	glm_mat4_identity(mat);
	VSR_Mat4* cubePosition = VSR_Mat4Create(renderer, (float *) mat);


	mat4 view;
	vec3 eye = {0.f, 0.f, 5.f};
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
	view[1][1] *= -1;

	VSR_PushConstants pushConstants;
	pushConstants.Projection = (VSR_Mat4*) &view;
	VSR_RendererSetVertexConstants(renderer, &pushConstants);
	VSR_RendererSetFragmentConstants(renderer, &pushConstants);

	// renderloop
	int shouldQuit = 0;
	SDL_Event event;

	int tick = 0;
	while(!shouldQuit)
	{
		if(tick == 0)
		{
			VSR_RendererSetRenderTarget(renderer, sampler2);
			tick++;
		}
		else if (tick == 1)
		{
			VSR_RendererSetRenderTarget(renderer, NULL);
			sampler1 = sampler2;
			tick++;
		}

		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, cubeModel, &cubePosition, &sampler1, 1);
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

#ifndef main
int main(void) 
{
	return SDL_main(0, NULL);
}
#endif