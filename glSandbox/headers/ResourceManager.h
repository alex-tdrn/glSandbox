#pragma once
#include <vector>
#include <memory>

template<typename T>
class ResourceManager
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

void initializeResources();
void drawResourcesUI(bool* open);
