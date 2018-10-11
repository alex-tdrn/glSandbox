#pragma once
#include "Shader.h"
#include "Scene.h"
#include "Cubemap.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

#include <vector>

namespace detail
{
	template<typename T>
	class ResourceManagerBase
	{
	private:
		static std::vector<std::unique_ptr<T>>& _getAll()
		{
			static std::vector<std::unique_ptr<T>> container;
			return container;
		}
	public:
		static std::vector<std::unique_ptr<T>> const& getAll()
		{
			return _getAll();
		}
		static T* add(std::unique_ptr<T>&& resource)
		{
			T* ret = resource.get();
			_getAll().push_back(std::move(resource));
			return ret;
		}
		static void add(std::vector<std::unique_ptr<T>>&& resources)
		{
			_getAll().reserve(_getAll().size() + resources.size());
			for(auto& resource : resources)
				_getAll().push_back(std::move(resource));
		}

	};
}

template<typename T>
class ResourceManager : public detail::ResourceManagerBase<T>
{
public:
	using detail::ResourceManagerBase<T>::getAll;
	using detail::ResourceManagerBase<T>::add;
};

template<>
class ResourceManager<Mesh> : public detail::ResourceManagerBase<Mesh>
{
public:
	static Mesh* quad();
	static Mesh* box();
	static Mesh* boxWireframe();
};

template<>
class ResourceManager<Texture> : public detail::ResourceManagerBase<Texture>
{
public:
	static Texture* debug();
};

template<>
class ResourceManager<Material> : public detail::ResourceManagerBase<Material>
{
public:
	static Material* basic();
};

template<>
class ResourceManager<Cubemap> : public detail::ResourceManagerBase<Cubemap>
{
public:
	static Cubemap* skybox();
};

template<>
class ResourceManager<Scene> : public detail::ResourceManagerBase<Scene>
{
public:
	static Scene* importGLTF(std::string_view const filename);
	static Scene* test();

};

template<>
class ResourceManager<Shader> : public detail::ResourceManagerBase<Shader>
{
public:
	static void reloadAll();
	static bool isLightingShader(Shader*);
	static Shader* unlit();
	static Shader* pbr();
	static Shader* blinnPhong();
	static Shader* phong();
	static Shader* gouraud();
	static Shader* flat();
	static Shader* reflection();
	static Shader* refraction();
	static Shader* debugNormals();
	static Shader* debugNormalsShowLines();
	static Shader* debugTexCoords();
	static Shader* debugDepthBuffer();
	static Shader* skybox();
	static Shader* gammaHDR();
	static Shader* passthrough();
	static Shader* grayscale();
	static Shader* chromaticAberration();
	static Shader* invert();
	static Shader* convolution();
};


void initializeResources();
void drawResourcesUI(bool* open);
