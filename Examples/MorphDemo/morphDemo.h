#include <VSR.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "../helpers.h"

static inline int
tweenMeshs(
	size_t vertexCount,
	VSR_Vertex* from,
	VSR_Vertex* to,
	VSR_Vertex* tween,
	float factor)
{

	size_t runs = vertexCount;
	while(runs--)
	{

		tween[runs].x = (1 - factor) * from[runs].x + factor * to[runs].x;
		tween[runs].y = (1 - factor) * from[runs].y + factor * to[runs].y;
		tween[runs].z = (1 - factor) * from[runs].z + factor * to[runs].z;
	}

	return 0;
}

static inline int
morphDemo(int argc, char *argv[])
{
	// setup sdl
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("TestWindow", 0,0,640,480, SDL_WINDOW_VULKAN);

	// setup renderer
	VSR_RendererCreateInfo* createInfo = VSR_RendererGenerateCreateInfo(window);
	VSR_Renderer* renderer = VSR_RendererCreate(createInfo);


	Mesh rest = loadObj("Assets\\JerryTheOgre\\bs_rest.obj", "Assets\\JerryTheOgre\\");

	VSR_Mesh* restMesh = VSR_MeshCreate(
		rest.numVertices,
		(const VSR_Vertex*) rest.vertices,
		(const VSR_Vertex*) rest.normals,
		(const VSR_UV*) rest.texcoords,
		rest.numIndices,
		(const VSR_Index*) rest.indices
	);

	VSR_Mesh* tweenMesh = VSR_MeshCreate(
		rest.numVertices,
		(const VSR_Vertex*) rest.vertices,
		(const VSR_Vertex*) rest.normals,
		(const VSR_UV*) rest.texcoords,
		rest.numIndices,
		(const VSR_Index*) rest.indices
	);

	Mesh smile = loadObj("Assets\\JerryTheOgre\\bs_smile.obj", "Assets\\JerryTheOgre\\");
	// create models
	VSR_Mesh* smileMesh = VSR_MeshCreate(
		smile.numVertices,
		(const VSR_Vertex*) smile.vertices,
		(const VSR_Vertex*) smile.normals,
		(const VSR_UV*) smile.texcoords,
		smile.numIndices,
		(const VSR_Index*) smile.indices
	);





	VSR_Model* terryModel = VSR_ModelCreate(renderer, tweenMesh);
	mat4 terryPos;
	glm_mat4_identity(terryPos);

	// set images
	SDL_Surface* sur1 = SDL_LoadBMP("Assets\\JerryTheOgre\\diffuse.bmp");
	SDL_Surface* sur2 = SDL_LoadBMP("Assets\\JerryTheOgre\\normalmap.bmp");
	VSR_Sampler* sampler1 = VSR_SamplerCreate(renderer, 1, sur1, 0);
	VSR_SamplerCreate(renderer, 2, sur2, 0);

	// create and set pipeline
	VSR_GraphicsPipelineCreateInfo* pipelineCreateInfo = VSR_GraphicsPipelineGenerateCreateInfo(renderer);
	size_t n;
	uint8_t* bytes = loadShader("MorphDemo\\vert.spv", &n);
	VSR_Shader* vert = VSR_ShaderCreate(renderer, n, bytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_VERTEX, vert);

	bytes = loadShader("MorphDemo\\frag.spv", &n);
	VSR_Shader* frag = VSR_ShaderCreate(renderer, n, bytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_FRAGMENT, frag);


	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);

	VSR_RendererSetPipeline(renderer, pipeline);

	mat4 mat;
	glm_mat4_identity(mat);
	glm_rotate(mat, 0.1f, (vec3){0.f, 0.f, 1.f});
	VSR_Mat4* terryPosition = VSR_Mat4Create(renderer, (float *) mat);


	/// setup projection ///
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

	VSR_Mat4* viewMat = VSR_Mat4Create(renderer, view);
	VSR_PushConstants pushConstants;
	pushConstants.Projection =  &viewMat;
	pushConstants.bytes = SDL_malloc(64);
	VSR_GraphicsPipelineSetPushConstants(renderer, pipeline, &pushConstants);

	// renderloop
	int shouldQuit = 0;
	SDL_Event event;
	float factor = 0;
	float rate = 0.013;
	while(!shouldQuit)
	{
		tweenMeshs(
			tweenMesh->vertexCount,
			restMesh->vertices,
			smileMesh->vertices,
			tweenMesh->vertices,
			factor);
		factor += rate;
		// ...
		VSR_ModelUpdate(renderer, terryModel, tweenMesh);

		VSR_RendererBeginPass(renderer);
		VSR_RenderModels(renderer, terryModel, &terryPosition, &sampler1, 1);
		VSR_RendererEndPass(renderer);

		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT) {shouldQuit = 1;}

	}

	// cleanup
	SDL_free(pushConstants.bytes);
	VSR_SamplerFree(renderer, sampler1);
	VSR_MeshFree(smileMesh);
	VSR_MeshFree(restMesh);
	VSR_ModelFree(renderer, terryModel);
	VSR_GraphicsPipelineCreateInfoFree(pipelineCreateInfo);
	VSR_GraphicsPipelineFree(renderer, pipeline);
	VSR_RendererFreeCreateInfo(createInfo);
	VSR_RendererFree(renderer);
	printf("exit successful");
	return 0;
}