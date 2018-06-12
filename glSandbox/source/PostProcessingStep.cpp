#include "Util.h"
#include "PostProcessingStep.h"
#include "Globals.h"

void PostProcessingStep::initFramebuffer()
{
	initialized = true;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, info::windowWidth, info::windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingStep::updateFramebuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, info::windowWidth, info::windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Shader& PostProcessingStep::getActiveShader()
{
	Shader& ret = [&]() -> Shader&{
		switch(active)
		{
			case type::grayscale:
				return resources::shaders::grayscale;
			case type::chromaticAberration:
				return resources::shaders::chromaticAberration;
			case type::invert:
				return resources::shaders::invert;
			case type::convolution:
				return resources::shaders::convolution;
			default:
				return resources::shaders::passthrough;
		}
	}();
	ret.use();
	ret.set("screenTexture", 0);

	if(active == type::convolution)
	{
		ret.set("offset", convolutionOffset);
		ret.set("divisor", convolutionDivisor);
		for(int i = 0; i < 9; i++)
			ret.set("kernel[" + std::to_string(i) + "]", convolutionKernel[i]);
	}
	if(active == type::chromaticAberration)
	{
		ret.set("intensity", chromaticAberrationIntensity);
		ret.set("offsetR", chromaticAberrationOffsetR);
		ret.set("offsetG", chromaticAberrationOffsetG);
		ret.set("offsetB", chromaticAberrationOffsetB);
	}
	return ret;
}

void PostProcessingStep::draw(unsigned int sourceColorbuffer, unsigned int targetFramebuffer)
{
	if(!initialized)
		initFramebuffer();
	bool doGammaHDR = false;
	if(targetFramebuffer == 0 && (settings::rendering::gammaCorrection || settings::rendering::HDR))
	{
		doGammaHDR = true;
		targetFramebuffer = framebuffer;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, targetFramebuffer);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	Shader& postprocessShader = getActiveShader();

	glBindVertexArray(resources::quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sourceColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	if(doGammaHDR)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		resources::shaders::gammaHDR.use();
		resources::shaders::gammaHDR.set("screenTexture", 0);
		if(settings::rendering::gammaCorrection)
			resources::shaders::gammaHDR.set("gamma", settings::rendering::gammaExponent);
		else
			resources::shaders::gammaHDR.set("gamma", 1.0f);

		if(settings::rendering::HDR)
			resources::shaders::gammaHDR.set("tonemapping", 1);
		else
			resources::shaders::gammaHDR.set("tonemapping", 0);

		glBindVertexArray(resources::quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void PostProcessingStep::draw(unsigned int sourceColorbuffer)
{
	if(!initialized)
		initFramebuffer();
	draw(sourceColorbuffer, framebuffer);
}

unsigned int PostProcessingStep::getColorbuffer()
{
	return colorbuffer;
}

void PostProcessingStep::drawUI()
{
	IDGuard idGuard{this};

	ImGui::Indent();
	ImGui::RadioButton("Passthrough", &active, type::passthrough);
	ImGui::SameLine();
	ImGui::RadioButton("Grayscale", &active, type::grayscale);
	ImGui::SameLine();
	ImGui::RadioButton("Chromatic Aberration", &active, type::chromaticAberration);

	ImGui::RadioButton("Invert", &active, type::invert);
	ImGui::SameLine();
	ImGui::RadioButton("Convolution", &active, type::convolution);
	switch(active)
	{
		case type::passthrough:
			break;
		case type::grayscale:
			break;
		case type::chromaticAberration:
			ImGui::DragFloat("Intensity", &chromaticAberrationIntensity, 0.01f);
			ImGui::DragFloat2("Offset R", &chromaticAberrationOffsetR.x, 0.0001f);
			ImGui::DragFloat2("Offset G", &chromaticAberrationOffsetG.x, 0.0001f);
			ImGui::DragFloat2("Offset B", &chromaticAberrationOffsetB.x, 0.0001f);
			break;
		case type::invert:
			break;
		case type::convolution:
			if(ImGui::Button("Sharpen"))
			{
				float kernel[9] = {
					0, -1, 0,
					-1, 5, -1,
					0, -1, 0
				};
				for(int i = 0; i < 9; i++)
					convolutionKernel[i] = kernel[i];
				convolutionDivisor = 1.0f;
			}
			ImGui::SameLine();
			if(ImGui::Button("Blur"))
			{
				float kernel[9] = {
					1, 2, 1,
					2, 4, 2,
					1, 2, 1
				};
				for(int i = 0; i < 9; i++)
					convolutionKernel[i] = kernel[i];
				convolutionDivisor = 16.0f;
			}
			ImGui::SameLine();
			if(ImGui::Button("Edge"))
			{
				float kernel[9] = {
					1, 1, 1,
					1, -8, 1,
					1, 1, 1
				};
				for(int i = 0; i < 9; i++)
					convolutionKernel[i] = kernel[i];
				convolutionDivisor = 1.0f;
			}
			ImGui::SameLine();
			if(ImGui::Button("Random"))
			{
				convolutionDivisor = 1.0f;
				for(int i = 0; i < 9; i++)
				{
					convolutionKernel[i] = (rand() % 100 - 50) / 10.0f;
					convolutionDivisor += convolutionKernel[i];
				}
				if(std::abs(convolutionDivisor) <= 0.01f)
					convolutionDivisor = 1.0f;
			}
			ImGui::DragFloat("Offset", &convolutionOffset, 0.0001f);
			ImGui::DragFloat("Divisor", &convolutionDivisor, 0.01f);
			ImGui::DragFloat3("##1", &convolutionKernel[0], 0.01f);
			ImGui::DragFloat3("##2", &convolutionKernel[3], 0.01f);
			ImGui::DragFloat3("##3", &convolutionKernel[6], 0.01f);
			break;
	}
	ImGui::Unindent();
}