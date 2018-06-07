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