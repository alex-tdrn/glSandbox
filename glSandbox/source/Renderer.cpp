#include "Renderer.h"
#include "Resources.h"
#include "Lights.h"
#include "Prop.h"

void Renderer::init()
{
	//TODO USE resize framebuffer here
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamples, GL_RGB16F, viewportWidth, viewportHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorbuffer, 0);

	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisamples, GL_DEPTH24_STENCIL8, viewportWidth, viewportHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &simpleFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &simpleColorbuffer);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, simpleColorbuffer, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::resizeFramebuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamples, GL_RGB16F, viewportWidth, viewportHeight, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisamples, GL_DEPTH24_STENCIL8, viewportWidth, viewportHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::render()
{
	if(resources::scenes.empty())
		return;
	auto const& props = resources::scenes[resources::activeScene].getActiveProps();
	auto const& camera = resources::scenes[resources::activeScene].getCamera();
	auto const& backgroundColor = resources::scenes[resources::activeScene].getBackground();
	auto const& directionalLights = resources::scenes[resources::activeScene].getDirectionalLights();
	auto const& spotLights = resources::scenes[resources::activeScene].getSpotLights();
	auto const& pointLights = resources::scenes[resources::activeScene].getPointLights();
	if(explicitRendering && !needRedraw)
		return;
	if(explicitRendering)
		needRedraw = false;
	else
		needRedraw = true;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);

	glEnable(GL_MULTISAMPLE);
	if(depthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(depthFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if(faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(faceCullingMode);
		glFrontFace(faceCullingOrdering);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}


	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	camera.use();
	resources::shaders::light.use();
	glBindVertexArray(resources::boxVAO);
	auto drawLights = [&](auto lights){
		for(auto const& light : lights)
		{
			if(!light.isEnabled())
				continue;
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light.getPosition());
			model = glm::scale(model, glm::vec3{0.2f});
			resources::shaders::light.set("model", model);
			resources::shaders::light.set("lightColor", light.getColor() * light.getIntensity());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	};
	drawLights(pointLights);
	drawLights(spotLights);
	Shader& activeShader = getActiveShader();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	if(isLightingShaderActive())
	{
		activeShader.set("ambientColor", backgroundColor);
		//directional lights
		{
			int enabledLights = 0;
			for(int i = 0; i < directionalLights.size(); i++)
			{
				if(!directionalLights[i].isEnabled())
					continue;
				std::string prefix = "dirLights[" + std::to_string(enabledLights) + "].";
				activeShader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4(directionalLights[i].getDirection(), 0.0f)));
				activeShader.set(prefix + "color", directionalLights[i].getColor());
				activeShader.set(prefix + "intensity", directionalLights[i].getIntensity());
				enabledLights++;
			}
			activeShader.set("nDirLights", enabledLights);
		}

		//point lights
		{
			int enabledLights = 0;
			for(int i = 0; i < pointLights.size(); i++)
			{
				if(!pointLights[i].isEnabled())
					continue;
				std::string prefix = "pointLights[" + std::to_string(enabledLights) + "].";
				activeShader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4(pointLights[i].getPosition(), 1.0f)));
				activeShader.set(prefix + "color", pointLights[i].getColor());
				activeShader.set(prefix + "intensity", pointLights[i].getIntensity());
				enabledLights++;
			}
			activeShader.set("nPointLights", enabledLights);
		}

		//spot lights
		{
			int enabledLights = 0;
			for(int i = 0; i < spotLights.size(); i++)
			{
				if(!spotLights[i].isEnabled())
					continue;
				std::string prefix = "spotLights[" + std::to_string(enabledLights) + "].";
				activeShader.set(prefix + "position", glm::vec3(viewMatrix * glm::vec4(spotLights[i].getPosition(), 1.0f)));
				activeShader.set(prefix + "direction", glm::vec3(viewMatrix * glm::vec4(spotLights[i].getDirection(), 0.0f)));
				activeShader.set(prefix + "color", spotLights[i].getColor());
				activeShader.set(prefix + "intensity", spotLights[i].getIntensity());
				activeShader.set(prefix + "innerCutoff", glm::cos(glm::radians(spotLights[i].getInnerCutoff())));
				activeShader.set(prefix + "outerCutoff", glm::cos(glm::radians(spotLights[i].getOuterCutoff())));
				enabledLights++;
			}
			activeShader.set("nSpotLights", enabledLights);
		}
	}
	else if(active == type::reflection || active == type::refraction)
	{
		/*if(skybox)
		{
			skybox->use();
			activeShader.set("skybox", 0);
			activeShader.set("cameraPos", camera.getPosition());
		}*/
	}
	else if(active == type::debugDepthBuffer)
	{
		activeShader.set("nearPlane", camera.getNearPlane());
		activeShader.set("farPlane", camera.getFarPlane());
	}
	else if(active == type::debugNormals)
	{
		if(debugNormalsShowLines)
		{

			//resources::shaders::debugNormalsShowLines.use();
			//resources::shaders::debugNormalsShowLines.set("model", model);

			//for(auto const& mesh : resources::meshes)
				//mesh.use();
			//activeShader.use();
		}
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glBindVertexArray(resources::boxVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for(auto const& prop : props)
	{
		activeShader.set("model", prop->getTransformation());
		activeShader.set("material.hasDiffuseMap", true);
		activeShader.set("material.diffuseMap", 1);
		activeShader.set("material.hasSpecularMap", false);
		activeShader.set("material.hasOpacityMap", false);
		resources::textures::placeholder.use(1);
		if(prop->getMeshIndex())
			resources::meshes[*prop->getMeshIndex()].use();
	}
	//for(auto& actor : actors)
	//actor.draw(activeShader);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	//resources::shaders::outline.use();
	//for(auto& actor : actors)
	//actor.drawOutline(resources::shaders::outline);
	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	//draw skybox
	/*if(skybox)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		skybox->use();
		resources::shaders::skybox.use();
		resources::shaders::skybox.set("skybox", 0);
		glBindVertexArray(resources::boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
unsigned int Renderer::getOutput()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, simpleFramebuffer);
	glBlitFramebuffer(0, 0, viewportWidth, viewportHeight, 0, 0, viewportWidth, viewportHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	return simpleColorbuffer;
}


Shader& Renderer::getActiveShader()
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

bool Renderer::isLightingShaderActive()
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


void Renderer::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Rendering", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	bool valueChanged = false;
	ImGui::Indent();
	ImGui::Checkbox("Explicit Rendering", &explicitRendering);
	ImGui::SameLine();
	if(ImGui::SliderInt("Samples", &multisamples, 1, 16))
		resizeFramebuffer();	
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
	//if(valueChanged)//TODO
	//resources::scenes::getActiveScene().update();
	ImGui::End();
}
