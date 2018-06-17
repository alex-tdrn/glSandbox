#pragma once
#include "Shader.h"
#include "Model.h"
#include "Scene.h"
#include "PostProcessingStep.h"
#include "Cubemap.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <vector>

namespace resources
{
	inline unsigned int boxVAO;
	inline unsigned int quadVAO;
	namespace scenes
	{
		inline Scene empty{"Empty"};
		inline Scene simple{"Simple"};
		inline Scene medium{"Medium"};
		inline Scene stressTest{"Stress Test"};
		namespace type
		{
			enum type
			{
				empty,
				simple,
				medium,
				stressTest
			};
		}
		inline int active = type::simple;

		inline Scene& getActiveScene();
	}
	namespace models
	{
		inline Model nanosuit{"models/nanosuit/nanosuit.obj", "Nanosuit"};
		inline Model bunny{"models/bunny/stanford-bunny.obj", "Bunny"};
		inline Model sponza{"models/sponza/sponza.obj", "Sponza"};
	}
	namespace shaders
	{
		//rendering
		inline Shader blinn_phong("shaders/phong.vert", "shaders/blinn-phong.frag");
		inline Shader phong("shaders/phong.vert", "shaders/phong.frag");
		inline Shader gouraud("shaders/gouraud.vert", "shaders/gouraud.frag");
		inline Shader flat("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
		inline Shader reflection("shaders/reflection.vert", "shaders/reflection.frag");
		inline Shader refraction("shaders/refraction.vert", "shaders/refraction.frag");
		inline Shader outline("shaders/outline.vert", "shaders/outline.frag");
		inline Shader debugNormals("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
		inline Shader debugNormalsShowLines("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
		inline Shader debugTexCoords("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
		inline Shader debugDepthBuffer("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
		inline Shader light("shaders/light.vert", "shaders/light.frag");
		inline Shader skybox("shaders/skybox.vert", "shaders/skybox.frag");
		//postprocessing
		inline Shader gammaHDR("shaders/pp.vert", "shaders/ppGammaHDR.frag");
		inline Shader passthrough("shaders/pp.vert", "shaders/ppPassthrough.frag");
		inline Shader grayscale("shaders/pp.vert", "shaders/ppBW.frag");
		inline Shader chromaticAberration("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
		inline Shader invert("shaders/pp.vert", "shaders/ppInvert.frag");
		inline Shader convolution("shaders/pp.vert", "shaders/ppConvolution.frag");

		inline void reload();
	}
	namespace textures
	{
		inline Texture placeholder("textures/placeholder.png", true);
	}
	namespace cubemaps
	{
		inline Cubemap skybox("skybox",{
			"cubemaps/skybox/right.jpg", "cubemaps/skybox/left.jpg",
			"cubemaps/skybox/top.jpg", "cubemaps/skybox/bottom.jpg",
			"cubemaps/skybox/front.jpg", "cubemaps/skybox/back.jpg"
			});
		inline Cubemap mp_blizzard("mp_blizzard",{
			"cubemaps/mp_blizzard/blizzard_rt.tga", "cubemaps/mp_blizzard/blizzard_lf.tga",
			"cubemaps/mp_blizzard/blizzard_up.tga", "cubemaps/mp_blizzard/blizzard_dn.tga",
			"cubemaps/mp_blizzard/blizzard_ft.tga", "cubemaps/mp_blizzard/blizzard_bk.tga"
			});
	}
	inline void init();
	inline void drawUI();
}

namespace info
{
	inline int windowWidth = 1920;
	inline int windowHeight = 960;
	inline void drawUI();
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
		inline void drawUI();
	}

	namespace postprocessing
	{
		inline std::vector<PostProcessingStep> steps(1);
		inline void drawUI();
	}
	inline void drawUI();
}

void resources::init()
{
	//create vertex data and buffer objects
	float boxVertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	unsigned int boxVBO;
	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	float quadVertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};


	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	for(Scene* scene : {&scenes::empty, &scenes::simple, &scenes::medium, &scenes::stressTest})
	{
		scene->setBackgroundColor({0.5f, 0.5f, 0.5f});
		scene->getCamera().setPosition(Position{{1.0f, 0.2f, 1.0f}});
		scene->getCamera().setOrientation(Orientation{215.0f, -5.0f, 0.0f});
	}

	{
		DirectionalLight light;
		light.setColor({1.0f, 1.0f, 1.0f});
		light.setOrientation({250.0f, -60.0f, 0.0f});
		scenes::simple.add(light);

		Actor actor(&resources::models::bunny);
		actor.setPosition({0.0f, 0.0f, 0.0f});
		actor.setScale({0.010f, 0.010f, 0.010f});
		scenes::simple.add(actor);

		scenes::simple.getCamera().setPosition(Position{{3.0f, 3.5f, 7.5f}});
		scenes::simple.getCamera().setOrientation(Orientation{235.0f, -25.0f, 0.0f});
	}

	{
		PointLight light;
		light.setColor({1.0f, 200.0f / 255.0f, 0.0f});
		light.setIntensity(5.0f);
		light.setPosition({-1.0f, 0.0f, 2.0f});
		scenes::medium.add(light);
	}
	{
		SpotLight light;
		light.setPosition({-10.0f, 0.0f, -1.5f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, -10.0f, 0.0f});
		light.setColor({0.0f, 1.0f, 0.0f});
		scenes::medium.add(light);

		light.setPosition({-10.0f, 0.0f, 1.0f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, -10.0f, 0.0f});
		light.setColor({1.0f, 0.0f, 0.0f});
		scenes::medium.add(light);

		light.setPosition({-10.0f, 2.0f, -0.25f});
		light.setIntensity(10.0f);
		light.setOrientation({180.0f, 0.0f, 0.0f});
		light.setColor({0.0f, 0.0f, 1.0f});
		scenes::medium.add(light);

		light.setPosition({-3.0f, 1.5f, 1.0f});
		light.setIntensity(400.0f);
		light.setOrientation({260.0f, -30.0f, 0.0f});
		light.setColor({1.0f, 1.0f, 1.0f});
		scenes::medium.add(light);
	}
	{
		Actor actor(&resources::models::sponza);
		actor.setPosition({0.0f, -1.8f, 0.0f});
		actor.setScale({0.0125f, 0.0125f, 0.0125f});
		scenes::medium.add(actor);
	}
	{
		int idx = 1;
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				Actor actor(&resources::models::bunny);
				actor.setPosition({0.0f - i * 4.0f, -0.75f, 0.0f - j * 2.0f});
				actor.setScale({0.005f, 0.005f, 0.005f});
				scenes::medium.add(actor);
				idx++;
			}
		}
	}

	{
		scenes::stressTest.getCamera().setPosition(Position{{3.0f, 2.0f, 1.0f}});
		scenes::stressTest.getCamera().setOrientation(Orientation{215.0f, -20.0f, 0.0f});
		DirectionalLight light;
		light.setOrientation({225.0f, +60.0f, 0.0f});

		scenes::stressTest.add(light);

		int idx = 1;
		const int n = 10;
		const float stride = 1.0f;
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				Actor actor(&resources::models::nanosuit);
				actor.setPosition({0.0f - i * stride, -1.75f, 0.0f - j * stride});
				actor.setScale({0.15f, 0.15f, 0.15f});
				scenes::stressTest.add(actor);
				idx++;
			}
		}
	}
}

Scene& resources::scenes::getActiveScene()
{
	Scene& ret = [&]() -> Scene&{
		switch(active)
		{
			case type::simple:
				return simple;
			case type::medium:
				return medium;
			case type::stressTest:
				return stressTest;
			default:
				return empty;
		}
	}();
	return ret;
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

void resources::shaders::reload()
{
	resources::shaders::blinn_phong.reload();
	resources::shaders::phong.reload();
	resources::shaders::gouraud.reload();
	resources::shaders::flat.reload();
	resources::shaders::reflection.reload();
	resources::shaders::refraction.reload();
	resources::shaders::outline.reload();
	resources::shaders::debugNormals.reload();
	resources::shaders::debugNormalsShowLines.reload();
	resources::shaders::debugTexCoords.reload();
	resources::shaders::debugDepthBuffer.reload();
	resources::shaders::light.reload();
	resources::shaders::skybox.reload();

	resources::shaders::gammaHDR.reload();
	resources::shaders::passthrough.reload();
	resources::shaders::grayscale.reload();
	resources::shaders::chromaticAberration.reload();
	resources::shaders::invert.reload();
	resources::shaders::convolution.reload();
}

void resources::drawUI()
{
	bool valueChanged = false;
	if(ImGui::CollapsingHeader("Resources"))
	{
		ImGui::Indent();
		if(ImGui::CollapsingHeader("Scenes"))
		{
			std::string_view comboPreview = scenes::empty.getName();
			switch(scenes::active)
			{
				case scenes::type::simple:
					comboPreview = scenes::simple.getName();
					break;
				case scenes::type::medium:
					comboPreview = scenes::medium.getName();
					break;
				case scenes::type::stressTest:
					comboPreview = scenes::stressTest.getName();
					break;
			}
			if(ImGui::BeginCombo("Active", comboPreview.data()))
			{
				if(ImGui::Selectable(scenes::empty.getName().data(), scenes::active == scenes::type::empty))
				{
					scenes::active = scenes::type::empty;
					valueChanged = true;
				}
				if(ImGui::Selectable(scenes::simple.getName().data(), scenes::active == scenes::type::simple))
				{
					scenes::active = scenes::type::simple;
					valueChanged = true;
				}
				if(ImGui::Selectable(scenes::medium.getName().data(), scenes::active == scenes::type::medium))
				{
					scenes::active = scenes::type::medium;
					valueChanged = true;
				}
				if(ImGui::Selectable(scenes::stressTest.getName().data(), scenes::active == scenes::type::stressTest))
				{
					scenes::active = scenes::type::stressTest;
					valueChanged = true;
				}
				ImGui::EndCombo();
			}
			scenes::getActiveScene().drawUI(false);
			scenes::empty.drawUI();
			scenes::simple.drawUI();
			scenes::medium.drawUI();
			scenes::stressTest.drawUI();
		}
		if(ImGui::CollapsingHeader("Models"))
		{
			valueChanged |= models::bunny.drawUI();
			valueChanged |= models::nanosuit.drawUI();
			valueChanged |= models::sponza.drawUI();
		}
		if(ImGui::CollapsingHeader("Cubemaps"))
		{
			valueChanged |= cubemaps::skybox.drawUI();
			valueChanged |= cubemaps::mp_blizzard.drawUI();
		}
		if(ImGui::Button("Reload Shaders"))
		{
			shaders::reload();
			valueChanged = true;
		}
		ImGui::Unindent();
	}
	if(valueChanged)
		scenes::getActiveScene().update();
}

void settings::drawUI()
{
	if(ImGui::CollapsingHeader("Settings"))
	{
		ImGui::Indent();
		rendering::drawUI();
		postprocessing::drawUI();
		ImGui::Unindent();
	}
}

void settings::rendering::drawUI()
{
	bool valueChanged = false;
	if(ImGui::CollapsingHeader("Rendering"))
	{
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
	}
	if(valueChanged)
		resources::scenes::getActiveScene().update();
}

void settings::postprocessing::drawUI()
{
	if(ImGui::CollapsingHeader("Post Processing"))
	{
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
	}
}

void info::drawUI()
{
	ImGui::Text("Window size %i, %i", windowWidth, windowHeight);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}