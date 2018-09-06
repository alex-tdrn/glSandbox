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
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
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
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcessingStep::draw(unsigned int sourceColorbuffer, unsigned int targetFramebuffer)
{
	if(!initialized)
		initFramebuffer();
	inputColorbuffer = sourceColorbuffer;
	bool doGammaHDR = false;
	/*if(targetFramebuffer == 0 && (settings::rendering::gammaCorrection || settings::rendering::tonemapping))
	{
		doGammaHDR = true;
		targetFramebuffer = framebuffer;
	}*/
	glBindFramebuffer(GL_FRAMEBUFFER, targetFramebuffer);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	Shader& currentShader = res::shaders()[currentShaderType];
	currentShader.use();
	currentShader.set("screenTexture", 0);

	switch(currentShaderType)
	{
		case res::ShaderType::convolution:
			currentShader.set("offset", convolutionOffset);
			currentShader.set("divisor", convolutionDivisor);
			for(int i = 0; i < 9; i++)
				currentShader.set("kernel[" + std::to_string(i) + "]", convolutionKernel[i]);
			break;
		case res::ShaderType::chromaticAberration:
			currentShader.set("intensity", chromaticAberrationIntensity);
			currentShader.set("offsetR", chromaticAberrationOffsetR);
			currentShader.set("offsetG", chromaticAberrationOffsetG);
			currentShader.set("offsetB", chromaticAberrationOffsetB);
			break;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sourceColorbuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	res::meshes::quad()->use();

	if(doGammaHDR)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		res::shaders()[res::ShaderType::gammaHDR].use();
		res::shaders()[res::ShaderType::gammaHDR].set("screenTexture", 0);
		/*if(settings::rendering::gammaCorrection)
			resources::shaders::gammaHDR.set("gamma", settings::rendering::gammaExponent);
		else
			resources::shaders::gammaHDR.set("gamma", 1.0f);
		if(settings::rendering::tonemapping)
		{
			resources::shaders::gammaHDR.set("tonemapping", settings::rendering::tonemapping);
			resources::shaders::gammaHDR.set("exposure", settings::rendering::exposure);
		}
		else
			resources::shaders::gammaHDR.set("tonemapping", 0);*/

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorbuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		res::meshes::quad()->use();
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
	//TODO
	//ImGui::Checkbox("Gamma Correction", &gammaCorrection);
	//if(gammaCorrection)
	//{
	//	ImGui::DragFloat("Gamma Exponent", &gammaExponent, 0.01f);
	//}
	//if(ImGui::CollapsingHeader("Tone Mapping"))
	//{
	//	ImGui::Indent();
	//
	//	ImGui::RadioButton("None", &tonemapping, tonemappingType::none);
	//	ImGui::SameLine();
	//	ImGui::RadioButton("Reinhard", &tonemapping, tonemappingType::reinhard);
	//	ImGui::SameLine();
	//	ImGui::RadioButton("Uncharted 2", &tonemapping, tonemappingType::uncharted2);
	//	ImGui::SameLine();
	//	ImGui::RadioButton("Hejl Burgess-Dawson", &tonemapping, tonemappingType::hejl_burgess_dawson);
	//	if(tonemapping)
	//	{
	//		ImGui::DragFloat("Exposure", &exposure, 0.1f);
	//	}
	//	ImGui::Unindent();
	//}
/*
	IDGuard idGuard{this};
	if(!floatImage)
	{
		float size = 256;
		if(ImGui::ImageButton(ImTextureID(inputColorbuffer), ImVec2(size, size / info::aspectRatio), ImVec2(0, 1), ImVec2(1, 0)))
			floatImage = true;
	}
	else
	{
		ImGui::Begin("buff", &floatImage, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
		const float titlebarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
		ImGui::SetWindowSize({ImGui::GetWindowSize().x, ImGui::GetWindowSize().x / info::aspectRatio + titlebarHeight + ImGui::GetStyle().WindowPadding.y});
		float size = ImGui::GetWindowContentRegionWidth();
		ImGui::Image(ImTextureID(inputColorbuffer), ImVec2(size, size / info::aspectRatio), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}
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
	}*/
}