#pragma once
#include "Shader.h"
#include "Resources.h"

class PostProcessingStep
{
private:
	bool initialized = false;
	unsigned int inputColorbuffer;
	unsigned int framebuffer;
	unsigned int colorbuffer;
	int currentShaderType = resources::ShaderType::passthrough;
	float convolutionKernel[9];
	float convolutionDivisor = 1.0f;
	float convolutionOffset = 0.001f;
	float chromaticAberrationIntensity = 1.0f;
	glm::vec2 chromaticAberrationOffsetR{0.0f};
	glm::vec2 chromaticAberrationOffsetG{0.001f, 0.0f};
	glm::vec2 chromaticAberrationOffsetB{0.0f, 0.001f};
	bool floatImage = false;
private:
	void initFramebuffer();

public:
	void updateFramebuffer();
	void draw(unsigned int sourceColourbuffer, unsigned int targetFramebuffer);
	void draw(unsigned int sourceColourbuffer);
	unsigned int getColorbuffer();
	void drawUI();
};