#include "Renderer.h"
#include "Resources.h"
#include "Lights.h"
#include "Prop.h"

Renderer::Renderer()
{
	glGenTextures(1, &multisampledColorbuffer);
	glGenRenderbuffers(1, &multisampledRenderbuffer);
	glGenTextures(1, &simpleColorbuffer);
	glGenRenderbuffers(1, &simpleRenderbuffer);
	updateFramebuffers();

	glGenFramebuffers(1, &multisampledFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampledRenderbuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &simpleFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, simpleColorbuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, simpleRenderbuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &multisampledColorbuffer);
	glDeleteTextures(1, &simpleColorbuffer);

	glDeleteRenderbuffers(1, &multisampledRenderbuffer);
	glDeleteRenderbuffers(1, &simpleRenderbuffer);

	glDeleteFramebuffers(1, &multisampledFramebuffer);
	glDeleteFramebuffers(1, &simpleFramebuffer);
}

void Renderer::updateFramebuffers()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, pipeline.samples > 0 && pipeline.samples < 16? pipeline.samples : 1, GL_RGB16F, viewport.width, viewport.height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRenderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, pipeline.samples > 0 && pipeline.samples < 16 ? pipeline.samples : 1, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, simpleColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, simpleRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::resizeViewport(int width, int height)
{
	viewport.width = width;
	viewport.height = height;
	updateFramebuffers();
}

void Renderer::render(Scene const& scene)
{
	if(explicitRendering && !needRedraw)
		return;
	if(explicitRendering)
		needRedraw = false;
	else
		needRedraw = true;

	if(pipeline.samples > 0)
	{
		glEnable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_FRAMEBUFFER, multisampledFramebuffer);
	}
	else
	{
		glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_FRAMEBUFFER, simpleFramebuffer);
	}
	if(pipeline.depthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(pipeline.depthFunction);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
	if(pipeline.faceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(pipeline.faceCullingMode);
		glFrontFace(pipeline.faceCullingOrdering);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	if(pipeline.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(scene.getBackground().r, scene.getBackground().g, scene.getBackground().b, 1.0f);

	auto const& props = scene.getActiveProps();
	auto const& camera = scene.getCamera();
	auto const& directionalLights = scene.getDirectionalLights();
	auto const& spotLights = scene.getSpotLights();
	auto const& pointLights = scene.getPointLights();

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

	Shader& activeShader = resources::shaders::get(shading.active);
	activeShader.use();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	switch(shading.active)
	{
		case resources::shaders::type::blinn_phong:
		case resources::shaders::type::phong:
		case resources::shaders::type::gouraud:
		case resources::shaders::type::flat:
			activeShader.set("material.overrideDiffuse", shading.lighting.override.diffuse);
			activeShader.set("material.overrideSpecular", shading.lighting.override.specular);
			activeShader.set("material.overrideDiffuseColor", shading.lighting.override.diffuseColor);
			activeShader.set("material.overrideSpecularColor", shading.lighting.override.specularColor);
			activeShader.set("ambientStrength", shading.lighting.ambientStrength);
			activeShader.set("material.shininess", shading.lighting.shininess);
			activeShader.set("ambientColor", scene.getBackground());
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
			break;
		case resources::shaders::type::refraction:
			activeShader.set("perChannel", shading.refraction.perChannel);
			activeShader.set("n1", shading.refraction.n1);
			activeShader.set("n1RGB", shading.refraction.n1RGB);
			activeShader.set("n2", shading.refraction.n2);
			activeShader.set("n2RGB", shading.refraction.n2RGB);
			[[fallthrough]];
		case resources::shaders::type::reflection:
			/*if(skybox)
			{
				skybox->use();
				activeShader.set("skybox", 0);
				activeShader.set("cameraPos", camera.getPosition());
			}*/
			break;
		case resources::shaders::type::debugDepthBuffer:
			activeShader.set("linearize", shading.debugging.depthBufferLinear);
			activeShader.set("nearPlane", camera.getNearPlane());
			activeShader.set("farPlane", camera.getFarPlane());
			break;
		case resources::shaders::type::debugNormals:
			if(shading.debugging.normals.showLines)
			{

				//resources::shaders::debugNormalsShowLines.use();
				//resources::shaders::debugNormalsShowLines.set("model", model);

				//for(auto const& mesh : resources::meshes)
					//mesh.use();
				//activeShader.use();
			}
			activeShader.set("viewSpace", shading.debugging.normals.viewSpace);
			activeShader.set("faceNormals", shading.debugging.normals.faceNormals);
			activeShader.set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
			if(shading.debugging.normals.showLines)
			{
				resources::shaders::debugNormalsShowLines.use();
				resources::shaders::debugNormalsShowLines.set("lineLength", shading.debugging.normals.lineLength);
				resources::shaders::debugNormalsShowLines.set("color", shading.debugging.normals.lineColor);
				resources::shaders::debugNormalsShowLines.set("viewSpace", shading.debugging.normals.viewSpace);
				resources::shaders::debugNormalsShowLines.set("faceNormals", shading.debugging.normals.faceNormals);
				resources::shaders::debugNormalsShowLines.set("explodeMagnitude", shading.debugging.normals.explodeMagnitude);
				activeShader.use();
			}
			break;
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
	if(pipeline.samples > 0)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFramebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, simpleFramebuffer);
		glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	return simpleColorbuffer;
}

void Renderer::drawUI(bool* open)
{
	//if(!*open)
	//	return;
	//ImGui::Begin("Rendering", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	//bool valueChanged = false;
	//ImGui::Indent();
	//ImGui::Checkbox("Explicit Rendering", &explicitRendering);
	//if(ImGui::InputInt("Samples", &multisamples))
	//	updateFramebuffers();	
	//valueChanged |= ImGui::Checkbox("Wireframe", &wireframe);
	//ImGui::SameLine();
	//valueChanged |= ImGui::Checkbox("Depth Testing", &depthTesting);
	//if(depthTesting && ImGui::CollapsingHeader("Depth Function"))
	//{
	//	ImGui::Indent();
	//	valueChanged |= ImGui::RadioButton("GL_ALWAYS", &depthFunction, GL_ALWAYS);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_NEVER", &depthFunction, GL_NEVER);
	//	valueChanged |= ImGui::RadioButton("GL_LESS", &depthFunction, GL_LESS);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_EQUAL", &depthFunction, GL_EQUAL);
	//	valueChanged |= ImGui::RadioButton("GL_LEQUAL", &depthFunction, GL_LEQUAL);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_GREATER", &depthFunction, GL_GREATER);
	//	valueChanged |= ImGui::RadioButton("GL_NOTEQUAL", &depthFunction, GL_NOTEQUAL);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_GEQUAL", &depthFunction, GL_GEQUAL);
	//	ImGui::Unindent();
	//}
	//valueChanged |= ImGui::Checkbox("Face Culling", &faceCulling);

	//if(faceCulling)
	//{
	//	ImGui::Indent();
	//	ImGui::Text("Face Culling Mode");
	//	valueChanged |= ImGui::RadioButton("GL_BACK", &faceCullingMode, GL_BACK);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_FRONT", &faceCullingMode, GL_FRONT);
	//	valueChanged |= ImGui::RadioButton("GL_FRONT_AND_BACK", &faceCullingMode, GL_FRONT_AND_BACK);
	//	ImGui::Text("Face Culling Ordering");
	//	valueChanged |= ImGui::RadioButton("GL_CCW", &faceCullingOrdering, GL_CCW);
	//	ImGui::SameLine();
	//	valueChanged |= ImGui::RadioButton("GL_CW ", &faceCullingOrdering, GL_CW);
	//	ImGui::Unindent();
	//}
	//valueChanged |= ImGui::RadioButton("Blinn-Phong", &active, type::blinn_phong);
	//ImGui::SameLine();
	//valueChanged |= ImGui::RadioButton("Phong", &active, type::phong);
	//valueChanged |= ImGui::RadioButton("Gouraud", &active, type::gouraud);
	//ImGui::SameLine();
	//valueChanged |= ImGui::RadioButton("Flat", &active, type::flat);
	//valueChanged |= ImGui::RadioButton("Reflection", &active, type::reflection);
	//ImGui::SameLine();
	//valueChanged |= ImGui::RadioButton("Refraction", &active, type::refraction);
	//valueChanged |= ImGui::RadioButton("Normals", &active, type::debugNormals);
	//ImGui::SameLine();
	//valueChanged |= ImGui::RadioButton("Texture Coordinates", &active, type::debugTexCoords);
	//valueChanged |= ImGui::RadioButton("Depth Buffer", &active, type::debugDepthBuffer);

	//switch(active)
	//{
	//	case type::blinn_phong:
	//	case type::phong:
	//	case type::gouraud:
	//	case type::flat:
	//		valueChanged |= ImGui::Checkbox("Override Diffuse", &overrideDiffuse);
	//		if(overrideDiffuse)
	//		{
	//			ImGui::SameLine();
	//			valueChanged |= ImGui::ColorEdit3("Diffuse", &overrideDiffuseColor.x, ImGuiColorEditFlags_NoInputs);

	//		}
	//		valueChanged |= ImGui::Checkbox("Override Specular", &overrideSpecular);
	//		if(overrideSpecular)
	//		{
	//			ImGui::SameLine();
	//			valueChanged |= ImGui::ColorEdit3("Specular", &overrideSpecularColor.x, ImGuiColorEditFlags_NoInputs);
	//		}
	//		valueChanged |= ImGui::SliderFloat("Shininess", &shininess, 0, 128);
	//		valueChanged |= ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
	//		break;
	//	case type::refraction:
	//		ImGui::Checkbox("Per Channel", &refractionPerChannel);
	//		if(!refractionPerChannel)
	//		{
	//			valueChanged |= ImGui::DragFloat("First Medium", &refractionN1, 0.001f);
	//			valueChanged |= ImGui::DragFloat("Second Medium", &refractionN2, 0.001f);
	//			refractionN1RGB = glm::vec3(refractionN1);
	//			refractionN2RGB = glm::vec3(refractionN2);
	//		}
	//		else
	//		{
	//			valueChanged |= ImGui::DragFloat3("First Medium", &refractionN1RGB.x, 0.001f);
	//			valueChanged |= ImGui::DragFloat3("Second Medium", &refractionN2RGB.x, 0.001f);
	//		}
	//		break;
	//	case type::debugNormals:
	//		valueChanged |= ImGui::Checkbox("View Space", &debugNormalsViewSpace);
	//		ImGui::SameLine();
	//		valueChanged |= ImGui::Checkbox("Show Lines", &debugNormalsShowLines);
	//		ImGui::SameLine();
	//		valueChanged |= ImGui::Checkbox("Face Normals", &debugNormalsFaceNormals);
	//		valueChanged |= ImGui::DragFloat("Explode Magnitude", &debugNormalsExplodeMagnitude, 0.01f);
	//		if(debugNormalsShowLines)
	//		{
	//			valueChanged |= ImGui::DragFloat("Line length", &debugNormalsLineLength, 0.001f);
	//			valueChanged |= ImGui::ColorEdit3("Line color", &debugNormalsLineColor.x, ImGuiColorEditFlags_NoInputs);
	//		}
	//		break;
	//	case type::debugTexCoords:

	//		break;
	//	case type::debugDepthBuffer:
	//		valueChanged |= ImGui::Checkbox("Linearize", &debugDepthBufferLinear);
	//		break;
	//}
	//ImGui::Unindent();
	////if(valueChanged)//TODO
	////resources::scenes::getActiveScene().update();
	//ImGui::End();
}
