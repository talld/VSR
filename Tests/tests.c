#include <VSR.h>
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

	uint8_t* fragBytes = loadShader("frag.spv", &n);
	VSR_Shader* fragShader = VSR_ShaderCreate(renderer, n, fragBytes);
	VSR_GraphicsPipelineSetShader(pipelineCreateInfo, SHADER_STAGE_FRAGMENT, fragShader);

	VSR_GraphicsPipeline* pipeline = VSR_GraphicsPipelineCreate(renderer, pipelineCreateInfo);

	VSR_RendererSetPipeline(renderer, pipeline);

	float verts[9] = {
		 0.0f,  -0.5f, 0.0f,
		 0.5f,   0.5f, 0.0f,
		-0.5f,   0.5f, 0.0f,};

	VSR_Model* model =
	VSR_ModelCreate(renderer, verts, 3, NULL, 0);

	int shouldQuit = 0;
	SDL_Event event;
	while(!shouldQuit)
	{

		model->vertices[0].x += 0.00001f;
		model->vertices[1].x += 0.00001f;
		model->vertices[2].x += 0.00001f;

		VSR_RendererBeginPass(renderer);
		VSR_ModelUpdate(renderer, model);
		VSR_RenderModels(renderer, model, NULL, 1);
		VSR_RendererEndPass(renderer);

		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			shouldQuit = 1;
		}
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