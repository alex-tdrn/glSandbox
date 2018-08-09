#pragma once
#include <string>

template <typename T>
class NamedAsset
{
private:
	std::string prefix;
	inline static int ct{1};

protected:
	std::string name;

public:
	NamedAsset(std::string prefix)
		: prefix(std::move(prefix))
	{
		name = prefix + '#' + std::to_string(ct++);
	}

	void setName(std::string name)
	{
		this->name = std::move(name);
	}

	std::string_view getName() const
	{
		return name;
	}

};
