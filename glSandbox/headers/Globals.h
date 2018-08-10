#pragma once

#include "Resources.h"
#include "PostProcessingStep.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>


namespace info
{
	inline int windowWidth = 1920;
	inline int windowHeight = 960;
	inline float aspectRatio = float(windowWidth) / windowHeight;
	inline bool rendering = false;
	inline void drawUI(bool* open);
}

namespace settings
{

	namespace rendering
	{
		enum type
		{
			blinn_phong,
			phong,
			gouraud,
			flat,
			reflection,
			refraction,
			debugNormals,
			debugTexCoords,
			debugDepthBuffer
		};
		inline int active = type::blinn_phong;	

		enum tonemappingType
		{
			none,
			reinhard,
			uncharted2,
			hejl_burgess_dawson
		};
		inline int multisamples = 1;
		inline bool vsync = true;
		inline bool gammaCorrection = true;
		inline int tonemapping = tonemappingType::none;
		inline float exposure = 1.0f;
		inline float gammaExponent = 2.2f;
		inline bool wireframe = false;
		inline bool depthTesting = true;
		inline bool faceCulling = false;
		inline bool explicitRendering = true;
		inline int depthFunction = GL_LESS;
		inline int faceCullingMode = GL_BACK;
		inline int faceCullingOrdering = GL_CCW;
		inline bool overrideDiffuse = false;
		inline bool overrideSpecular = false;
		inline glm::vec3 overrideDiffuseColor{1.0f, 1.0f, 1.0f};
		inline glm::vec3 overrideSpecularColor{1.0f, 1.0f, 1.0f};
		inline float ambientStrength = 0.025f;
		inline float shininess = 8;
		inline bool refractionPerChannel = false;
		inline float refractionN1 = 1.0f;
		inline glm::vec3 refractionN1RGB{refractionN1};
		inline float refractionN2 = 1.52f;
		inline glm::vec3 refractionN2RGB{refractionN2};
		inline bool debugDepthBufferLinear = false;
		inline bool debugNormalsViewSpace = false;
		inline bool debugNormalsFaceNormals= false;
		inline float debugNormalsExplodeMagnitude = 0.0f;
		inline bool debugNormalsShowLines = false;
		inline float debugNormalsLineLength = 0.015f;
		inline glm::vec3 debugNormalsLineColor{1.0f, 1.0f, 1.0f};

		inline Shader& getActiveShader();
		inline bool isLightingShaderActive();
		inline void drawUI(bool* open);
	}

	namespace postprocessing
	{
		inline std::vector<PostProcessingStep> steps(1);
		inline void drawUI(bool* open);
	}
}


Shader& settings::rendering::getActiveShader()
{
	Shader& ret = [&]() -> Shader&{
		switch(active)
		{
			case type::phong:
				return resources::shaders::phong;
			case type::gouraud:
				return resources::shaders::gouraud;
			case type::flat:
				return resources::shaders::flat;
			case type::reflection:
				return resources::shaders::reflection;
			case type::refraction:
				return resources::shaders::refraction;
			case type::debugNormals:
				return resources::shaders::debugNormals;
			case type::debugTexCoords:
				return resources::shaders::debugTexCoords;
			case type::debugDepthBuffer:
				return resources::shaders::debugDepthBuffer;
			default:
				return resources::shaders::blinn_phong;
		}
	}();
	ret.use();
	switch(active)
	{
		case type::blinn_phong:
		case type::phong:
		case type::gouraud:
		case type::flat:
			ret.set("material.overrideDiffuse", overrideDiffuse);
			ret.set("material.overrideSpecular", overrideSpecular);
			ret.set("material.overrideDiffuseColor", overrideDiffuseColor);
			ret.set("material.overrideSpecularColor", overrideSpecularColor);
			ret.set("ambientStrength", ambientStrength);
			ret.set("material.shininess", shininess);
			break;
		case type::refraction:
			ret.set("perChannel", refractionPerChannel);
			ret.set("n1", refractionN1);
			ret.set("n1RGB", refractionN1RGB);
			ret.set("n2", refractionN2);
			ret.set("n2RGB", refractionN2RGB);
			break;
		case type::debugDepthBuffer:
			ret.set("linearize", debugDepthBufferLinear);
			break;
		case type::debugNormals:
			ret.set("viewSpace", debugNormalsViewSpace);
			ret.set("faceNormals", debugNormalsFaceNormals);
			ret.set("explodeMagnitude", debugNormalsExplodeMagnitude);
			if(debugNormalsShowLines)
			{
				resources::shaders::debugNormalsShowLines.use();
				resources::shaders::debugNormalsShowLines.set("lineLength", debugNormalsLineLength);
				resources::shaders::debugNormalsShowLines.set("color", debugNormalsLineColor);
				resources::shaders::debugNormalsShowLines.set("viewSpace", debugNormalsViewSpace);
				resources::shaders::debugNormalsShowLines.set("faceNormals", debugNormalsFaceNormals);
				resources::shaders::debugNormalsShowLines.set("explodeMagnitude", debugNormalsExplodeMagnitude);
				ret.use();
			}
			break;
	}
	return ret;
}

bool settings::rendering::isLightingShaderActive()
{
	switch(active)
	{
		case type::blinn_phong:
		case type::phong:
		case type::gouraud:
		case type::flat:
			return true;
		default:
			return false;
	}
}


void settings::rendering::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Rendering", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	bool valueChanged = false;
	ImGui::Indent();
	ImGui::Checkbox("Explicit Rendering", &explicitRendering);
	ImGui::SameLine();
	ImGui::Checkbox("V-Sync", &vsync);
	if(ImGui::SliderInt("Samples", &multisamples,1, 16))
		resources::scenes::getActiveScene().updateFramebuffer();
	ImGui::Checkbox("Gamma Correction", &gammaCorrection);
	if(gammaCorrection)
	{
		ImGui::DragFloat("Gamma Exponent", &gammaExponent, 0.01f);
	}
	if(ImGui::CollapsingHeader("Tone Mapping"))
	{
		ImGui::Indent();

		ImGui::RadioButton("None", &tonemapping, tonemappingType::none);
		ImGui::SameLine();
		ImGui::RadioButton("Reinhard", &tonemapping, tonemappingType::reinhard);
		ImGui::SameLine();
		ImGui::RadioButton("Uncharted 2", &tonemapping, tonemappingType::uncharted2);
		ImGui::SameLine();
		ImGui::RadioButton("Hejl Burgess-Dawson", &tonemapping, tonemappingType::hejl_burgess_dawson);
		if(tonemapping)
		{
			ImGui::DragFloat("Exposure", &exposure, 0.1f);
		}
		ImGui::Unindent();
	}
	valueChanged |= ImGui::Checkbox("Wireframe", &wireframe);

	ImGui::SameLine();
	valueChanged |= ImGui::Checkbox("Depth Testing", &depthTesting);


	if(depthTesting && ImGui::CollapsingHeader("Depth Function"))
	{
		ImGui::Indent();
		valueChanged |= ImGui::RadioButton("GL_ALWAYS", &depthFunction, GL_ALWAYS);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_NEVER", &depthFunction, GL_NEVER);
		valueChanged |= ImGui::RadioButton("GL_LESS", &depthFunction, GL_LESS);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_EQUAL", &depthFunction, GL_EQUAL);
		valueChanged |= ImGui::RadioButton("GL_LEQUAL", &depthFunction, GL_LEQUAL);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_GREATER", &depthFunction, GL_GREATER);
		valueChanged |= ImGui::RadioButton("GL_NOTEQUAL", &depthFunction, GL_NOTEQUAL);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_GEQUAL", &depthFunction, GL_GEQUAL);
		ImGui::Unindent();
	}
	valueChanged |= ImGui::Checkbox("Face Culling", &faceCulling);

	if(faceCulling)
	{
		ImGui::Indent();
		ImGui::Text("Face Culling Mode");
		valueChanged |= ImGui::RadioButton("GL_BACK", &faceCullingMode, GL_BACK);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_FRONT", &faceCullingMode, GL_FRONT);
		valueChanged |= ImGui::RadioButton("GL_FRONT_AND_BACK", &faceCullingMode, GL_FRONT_AND_BACK);
		ImGui::Text("Face Culling Ordering");
		valueChanged |= ImGui::RadioButton("GL_CCW", &faceCullingOrdering, GL_CCW);
		ImGui::SameLine();
		valueChanged |= ImGui::RadioButton("GL_CW ", &faceCullingOrdering, GL_CW);
		ImGui::Unindent();
	}
	valueChanged |= ImGui::RadioButton("Blinn-Phong", &active, type::blinn_phong);
	ImGui::SameLine();
	valueChanged |= ImGui::RadioButton("Phong", &active, type::phong);
	valueChanged |= ImGui::RadioButton("Gouraud", &active, type::gouraud);
	ImGui::SameLine();
	valueChanged |= ImGui::RadioButton("Flat", &active, type::flat);
	valueChanged |= ImGui::RadioButton("Reflection", &active, type::reflection);
	ImGui::SameLine();
	valueChanged |= ImGui::RadioButton("Refraction", &active, type::refraction);
	valueChanged |= ImGui::RadioButton("Normals", &active, type::debugNormals);
	ImGui::SameLine();
	valueChanged |= ImGui::RadioButton("Texture Coordinates", &active, type::debugTexCoords);
	valueChanged |= ImGui::RadioButton("Depth Buffer", &active, type::debugDepthBuffer);

	switch(active)
	{
		case type::blinn_phong:
		case type::phong:
		case type::gouraud:
		case type::flat:
			valueChanged |= ImGui::Checkbox("Override Diffuse", &overrideDiffuse);
			if(overrideDiffuse)
			{
				ImGui::SameLine();
				valueChanged |= ImGui::ColorEdit3("Diffuse", &overrideDiffuseColor.x, ImGuiColorEditFlags_NoInputs);

			}
			valueChanged |= ImGui::Checkbox("Override Specular", &overrideSpecular);
			if(overrideSpecular)
			{
				ImGui::SameLine();
				valueChanged |= ImGui::ColorEdit3("Specular", &overrideSpecularColor.x, ImGuiColorEditFlags_NoInputs);
			}
			valueChanged |= ImGui::SliderFloat("Shininess", &shininess, 0, 128);
			valueChanged |= ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
			break;
		case type::refraction:
			ImGui::Checkbox("Per Channel", &refractionPerChannel);
			if(!refractionPerChannel)
			{
				valueChanged |= ImGui::DragFloat("First Medium", &refractionN1, 0.001f);
				valueChanged |= ImGui::DragFloat("Second Medium", &refractionN2, 0.001f);
				refractionN1RGB = glm::vec3(refractionN1);
				refractionN2RGB = glm::vec3(refractionN2);
			}
			else
			{
				valueChanged |= ImGui::DragFloat3("First Medium", &refractionN1RGB.x, 0.001f);
				valueChanged |= ImGui::DragFloat3("Second Medium", &refractionN2RGB.x, 0.001f);
			}
			break;
		case type::debugNormals:
			valueChanged |= ImGui::Checkbox("View Space", &debugNormalsViewSpace);
			ImGui::SameLine();
			valueChanged |= ImGui::Checkbox("Show Lines", &debugNormalsShowLines);
			ImGui::SameLine();
			valueChanged |= ImGui::Checkbox("Face Normals", &debugNormalsFaceNormals);
			valueChanged |= ImGui::DragFloat("Explode Magnitude", &debugNormalsExplodeMagnitude, 0.01f);
			if(debugNormalsShowLines)
			{
				valueChanged |= ImGui::DragFloat("Line length", &debugNormalsLineLength, 0.001f);
				valueChanged |= ImGui::ColorEdit3("Line color", &debugNormalsLineColor.x, ImGuiColorEditFlags_NoInputs);
			}
			break;
		case type::debugTexCoords:

			break;
		case type::debugDepthBuffer:
			valueChanged |= ImGui::Checkbox("Linearize", &debugDepthBufferLinear);
			break;
	}
	ImGui::Unindent();
	if(valueChanged)
		resources::scenes::getActiveScene().update();
	ImGui::End();
}

void settings::postprocessing::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Post Processing", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	if(ImGui::Button("Add step"))
		steps.emplace_back();
	int removeIdx = -1;
	for(int i = 0; i < steps.size(); i++)
	{
		if(ImGui::TreeNode(("Step " + std::to_string(i)).c_str()))
		{
			if(steps.size() > 1)
				if(ImGui::Button("Remove step"))
					removeIdx = i;
			steps[i].drawUI();
			ImGui::TreePop();
		}
	}
	if(removeIdx != -1)
		steps.erase(steps.begin() + removeIdx);

	ImGui::End();
}

void info::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Stats", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImGui::Text("Window size %i, %i", windowWidth, windowHeight);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Renderer: ");
	ImGui::SameLine();
	ImGui::Text(rendering ? "active" : "waiting");
	ImGui::End();
}