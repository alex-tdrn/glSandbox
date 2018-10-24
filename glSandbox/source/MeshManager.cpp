#include "MeshManager.h"


void MeshManager::initialize()
{
	quad();
	box();
	boxWireframe();
}

Mesh* MeshManager::quad()
{
	static Mesh* ret = []() -> Mesh*{
		std::vector<Vertex> vertices = {
			{//top left
				glm::vec3{-1.0f, +1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottm right
				glm::vec3{+1.0f, -1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +0.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +1.0f}
			}
		};
		std::vector<uint8_t> indices = {
			0, 1, 2,
			0, 2, 3
		};
		auto mesh = std::make_unique<Mesh>(
			calculateBounds(vertices), GL_TRIANGLES,
			buildAttributes(std::move(vertices)),
			buildIndexBuffer(std::move(indices)));
		mesh->setName("quad");
		return add(std::move(mesh));
	}();
	return ret;
}

Mesh* MeshManager::box()
{
	static Mesh* ret = []() -> Mesh*{
		std::vector<Vertex> vertices = {
			//front face
			{//top left
				glm::vec3{-1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +0.0f, +1.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//left face
			{//top left
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, +1.0f},
				glm::vec3{-1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//back face
			{//top left
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +0.0f, -1.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//right face
			{//top left
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+1.0f, +0.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//top face
			{//top left
				glm::vec3{-1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, +1.0f, +1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, -1.0f},
				glm::vec3{+0.0f, +1.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},

			//bottom face
			{//top left
				glm::vec3{+1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+1.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+1.0f, +0.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, +1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+0.0f, +0.0f}
			},
			{//top right
				glm::vec3{-1.0f, -1.0f, -1.0f},
				glm::vec3{+0.0f, -1.0f, +0.0f},
				glm::vec2{+0.0f, +1.0f}
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
		auto mesh = std::make_unique<Mesh>(
			calculateBounds(vertices), GL_TRIANGLES,
			buildAttributes(std::move(vertices)),
			buildIndexBuffer(std::move(indices)));
		mesh->setName("box");
		return add(std::move(mesh));
	}();
	return ret;
}

Mesh* MeshManager::boxWireframe()
{
	static Mesh* ret = []() -> Mesh*{
		std::vector<SimpleVertex> vertices = {
			//front face
			{//top left
				glm::vec3{-1.0f, +1.0f, +1.0f}
			},
			{//bottom left
				glm::vec3{-1.0f, -1.0f, +1.0f}
			},
			{//bottom right
				glm::vec3{+1.0f, -1.0f, +1.0f}
			},
			{//top right
				glm::vec3{+1.0f, +1.0f, +1.0f}
			},

			//back face
			{//top left
				glm::vec3{+1.0f, +1.0f, -1.0f}
			},
			{//bottom left
				glm::vec3{+1.0f, -1.0f, -1.0f}
			},
			{//bottom right
				glm::vec3{-1.0f, -1.0f, -1.0f}
			},
			{//top right
				glm::vec3{-1.0f, +1.0f, -1.0f}
			}
		};
		std::vector<uint8_t> indices = {
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			0, 7,
			7, 6,
			6, 1,
			2, 5,
			5, 4,
			4, 3,
			7, 4,
			5, 6
		};
		auto mesh = std::make_unique<Mesh>(
			calculateBounds(vertices), GL_LINES,
			buildAttributes(std::move(vertices)),
			buildIndexBuffer(std::move(indices)));
		mesh->setName("box_wireframe");
		return (add(std::move(mesh)));
	}();
	return ret;
}
