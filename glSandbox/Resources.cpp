#include "Resources.h"

#include <glad/glad.h>
#include <imgui.h>

void resources::init()
{
	struct Vertex
	{
		float const position[3];
		float const normal[3];
		float const texcoords[2];
	};
	Mesh::Attributes::AttributeBuffer positions;
	positions.attributeType = Mesh::AttributeType::positions;
	positions.componentSize = 3;
	positions.dataType = GL_FLOAT;
	positions.stride = sizeof(Vertex);
	positions.offset = offsetof(Vertex, position);

	Mesh::Attributes::AttributeBuffer normals;
	normals.attributeType = Mesh::AttributeType::normals;
	normals.componentSize = 3;
	normals.dataType = GL_FLOAT;
	normals.stride = sizeof(Vertex);
	normals.offset = offsetof(Vertex, normal);

	Mesh::Attributes::AttributeBuffer texcoords;
	texcoords.attributeType = Mesh::AttributeType::texcoords;
	texcoords.componentSize = 2;
	texcoords.dataType = GL_FLOAT;
	texcoords.stride = sizeof(Vertex);
	texcoords.offset = offsetof(Vertex, texcoords);

	//create vertex data and buffer objects
	{
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
	}

	//Quad
	{
		Vertex const vertices[] = {
			{
				-1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +1.0f
			},
			{
				-1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +0.0f
			},
			{
				+1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +0.0f
			},
			{
				+1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +1.0f
			}
		};
		unsigned short const indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		Mesh::Attributes attributes;
		attributes.array[Mesh::AttributeType::positions] = positions;
		attributes.array[Mesh::AttributeType::normals] = normals;
		attributes.array[Mesh::AttributeType::texcoords] = texcoords;
		attributes.interleaved = true;
		attributes.data = reinterpret_cast<uint8_t const*>(vertices);
		attributes.size = sizeof(vertices);
		Mesh::IndexBuffer indexBuffer;
		indexBuffer.data = reinterpret_cast<uint8_t const*>(indices);
		indexBuffer.count = 6;
		indexBuffer.size = sizeof(indices);
		indexBuffer.dataType = GL_UNSIGNED_SHORT;
		Bounds bounds{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 0.0f}};
		primitives::quad = std::make_unique<Mesh>(bounds, GL_TRIANGLES, std::move(attributes), indexBuffer);
	}

	for(int i = 0; i < ShaderType::END; i++)
	{
		switch(i)
		{
			case ShaderType::blinn_phong:
				shaders.emplace_back("shaders/phong.vert", "shaders/blinn-phong.frag");
				break;
			case ShaderType::phong:
				shaders.emplace_back("shaders/phong.vert", "shaders/phong.frag");
				break;
			case ShaderType::gouraud:
				shaders.emplace_back("shaders/gouraud.vert", "shaders/gouraud.frag");
				break;
			case ShaderType::flat:
				shaders.emplace_back("shaders/flat.vert", "shaders/flat.frag", "shaders/flat.geom");
				break;
			case ShaderType::reflection:
				shaders.emplace_back("shaders/reflection.vert", "shaders/reflection.frag");
				break;
			case ShaderType::refraction:
				shaders.emplace_back("shaders/refraction.vert", "shaders/refraction.frag");
				break;
			case ShaderType::outline:
				shaders.emplace_back("shaders/outline.vert", "shaders/outline.frag");
				break;
			case ShaderType::debugNormals:
				shaders.emplace_back("shaders/debugNormals.vert", "shaders/debugNormals.frag", "shaders/debugNormals.geom");
				break;
			case ShaderType::debugNormalsShowLines:
				shaders.emplace_back("shaders/debugNormalsShowLines.vert", "shaders/debugNormalsShowLines.frag", "shaders/debugNormalsShowLines.geom");
				break;
			case ShaderType::debugTexCoords:
				shaders.emplace_back("shaders/debugTextureCoords.vert", "shaders/debugTextureCoords.frag");
				break;
			case ShaderType::debugDepthBuffer:
				shaders.emplace_back("shaders/debugDepthBuffer.vert", "shaders/debugDepthBuffer.frag");
				break;
			case ShaderType::light:
				shaders.emplace_back("shaders/light.vert", "shaders/light.frag");
				break;
			case ShaderType::skybox:
				shaders.emplace_back("shaders/skybox.vert", "shaders/skybox.frag");
				break;
			case ShaderType::gammaHDR:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppGammaHDR.frag");
				break;
			case ShaderType::passthrough:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppPassthrough.frag");
				break;
			case ShaderType::grayscale:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppBW.frag");
				break;
			case ShaderType::chromaticAberration:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppChromaticAberration.frag");
				break;
			case ShaderType::invert:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppInvert.frag");
				break;
			case ShaderType::convolution:
				shaders.emplace_back("shaders/pp.vert", "shaders/ppConvolution.frag");
				break;
			default:
				assert(false);//means we forgot to add a shader
		}
	}
}

void resources::reloadShaders()
{
	for(int i = 0; i < ShaderType::END; i++)
		shaders[i].reload();
}

void resources::drawUI(bool* open)
{
	if(!*open)
		return;
	ImGui::Begin("Resources", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

	bool valueChanged = false;
	ImGui::Indent();
	if(ImGui::CollapsingHeader("Scenes"))
	{
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.25f);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		for(auto& scene : scenes)
			scene->drawUI();
	}
	if(ImGui::CollapsingHeader("Meshes"))
		for(auto& mesh : meshes)
			mesh.drawUI();
	if(ImGui::CollapsingHeader("Cubemaps"))
	{
		valueChanged |= cubemaps::skybox.drawUI();
		valueChanged |= cubemaps::mp_blizzard.drawUI();
	}
	if(ImGui::Button("Reload Shaders"))
	{
		reloadShaders();
		valueChanged = true;
	}
	ImGui::Unindent();
	//if(valueChanged)
	//scenes::getActiveScene().update();

	ImGui::End();
}
