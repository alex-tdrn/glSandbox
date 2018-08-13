#include "Resources.h"

#include <glad/glad.h>
#include <imgui.h>

struct Vertex
{
	float const position[3];
	float const normal[3];
	float const texcoords[2];
};
Mesh buildMesh(std::vector<Vertex>&& vertices, std::optional<std::vector<uint8_t>>&& indices = std::nullopt)
{
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

	Mesh::Attributes attributes;
	attributes.array[Mesh::AttributeType::positions] = positions;
	attributes.array[Mesh::AttributeType::normals] = normals;
	attributes.array[Mesh::AttributeType::texcoords] = texcoords;
	attributes.interleaved = true;
	attributes.data = reinterpret_cast<uint8_t const*>(vertices.data());
	attributes.size = vertices.size() * sizeof(Vertex);

	std::optional<Mesh::IndexBuffer> indexBuffer;
	if(indices)
	{
		indexBuffer.emplace();
		indexBuffer->data = reinterpret_cast<uint8_t const*>(indices->data());
		indexBuffer->count = 6;
		indexBuffer->size = indices->size() * sizeof(uint8_t);
		indexBuffer->dataType = GL_UNSIGNED_BYTE;
	}
	Bounds bounds;
	for(auto vertex : vertices)
		bounds += vertex.position;
	return {bounds, GL_TRIANGLES, std::move(attributes), std::move(indexBuffer)};
}

Mesh& resources::quad()
{
	static Mesh quad = []() -> Mesh{
		std::vector<Vertex> vertices = {
			{//top left
				-1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +0.0f
			},
			{//bottm right
				+1.0f, -1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, +0.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +1.0f
			}
		};
		std::vector<uint8_t> indices = {
			0, 1, 2,
			0, 2, 3
		};
		return buildMesh(std::move(vertices), std::move(indices));
	}();
	return quad;
}

Mesh& resources::box()
{
	static Mesh box = []() -> Mesh{
		std::vector<Vertex> vertices = {
			//front face
			{//top left
				-1.0f, +1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, -1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, +1.0f,
				+0.0f, +0.0f, +1.0f,
				+1.0f, +1.0f
			},

			//left face
			{//top left
				-1.0f, +1.0f, -1.0f,
				-1.0f, +0.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, -1.0f, -1.0f,
				-1.0f, +0.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				-1.0f, -1.0f, +1.0f,
				-1.0f, +0.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				-1.0f, +1.0f, +1.0f,
				-1.0f, +0.0f, +0.0f,
				+1.0f, +1.0f
			},
			
			//back face
			{//top left
				+1.0f, +1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				+1.0f, -1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				-1.0f, -1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+1.0f, +0.0f
			},
			{//top right
				-1.0f, +1.0f, -1.0f,
				+0.0f, +0.0f, -1.0f,
				+1.0f, +1.0f
			},
			
			//right face
			{//top left
				+1.0f, +1.0f, +1.0f,
				+1.0f, +0.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				+1.0f, -1.0f, +1.0f,
				+1.0f, +0.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, -1.0f, -1.0f,
				+1.0f, +0.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, -1.0f,
				+1.0f, +0.0f, +0.0f,
				+1.0f, +1.0f
			},
			
			//top face
			{//top left
				-1.0f, +1.0f, -1.0f,
				+0.0f, +1.0f, +0.0f,
				+0.0f, +1.0f
			},
			{//bottom left
				-1.0f, +1.0f, +1.0f,
				+0.0f, +1.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//bottom right
				+1.0f, +1.0f, +1.0f,
				+0.0f, +1.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//top right
				+1.0f, +1.0f, -1.0f,
				+0.0f, +1.0f, +0.0f,
				+1.0f, +1.0f
			},

			//bottom face
			{//top left
				+1.0f, +1.0f, -1.0f,
				+0.0f, -1.0f, +0.0f,
				+1.0f, +1.0f
			},
			{//bottom left
				+1.0f, +1.0f, +1.0f,
				+0.0f, -1.0f, +0.0f,
				+1.0f, +0.0f
			},
			{//bottom right
				-1.0f, +1.0f, +1.0f,
				+0.0f, -1.0f, +0.0f,
				+0.0f, +0.0f
			},
			{//top right
				-1.0f, +1.0f, -1.0f,
				+0.0f, -1.0f, +0.0f,
				+0.0f, +1.0f
			},
		};
		std::vector<uint8_t> indices = {
			//front face
			0, 1, 2,
			0, 2, 3,
			//left face
			4, 5, 6,
			4, 6, 7,
			//back face
			8, 9, 10,
			8, 10, 11,
			//right face
			12, 13, 14,
			12, 14, 15,
			//top face
			16, 17, 18,
			16, 18, 19,
			//bottom face
			20, 21, 22,
			20, 22, 23
		};
		return buildMesh(std::move(vertices), std::move(indices));
	}();
	return box;
}

void resources::loadShaders()
{
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
