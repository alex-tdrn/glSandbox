#pragma once
#include "Shader.h"

class PostProcessingStep
{
private:
	bool initialized = false;
	unsigned int framebuffer;
	unsigned int colorbuffer;
	enum type
	{
		passthrough,
		grayscale,
		chromaticAberration,
		invert,
		convolution
	};
	int active = type::passthrough;
	float convolutionKernel[9];
	float convolutionDivisor = 1.0f;
	float convolutionOffset = 0.001f;
	float chromaticAberrationIntensity = 1.0f;
	glm::vec2 chromaticAberrationOffsetR{0.0f};
	glm::vec2 chromaticAberrationOffsetG{0.001f, 0.0f};
	glm::vec2 chromaticAberrationOffsetB{0.0f, 0.001f};

private:
	void initFramebuffer();
	Shader& getActiveShader();

public:
	void updateFramebuffer();
	void draw(unsigned int sourceColourbuffer, unsigned int targetFramebuffer);
	void draw(unsigned int sourceColourbuffer);
	unsigned int getColorbuffer();
	void drawUI();
};