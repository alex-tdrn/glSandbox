#pragma once
#include <string>

template <typename T>
class Asset
{
private:
	std::string const namePrefix;
	inline static int ct{1};

protected:
	std::string name;

public:
	Asset(std::string namePrefix)
		: namePrefix(std::move(namePrefix))
	{
		name = namePrefix + '#' + std::to_string(ct++);
	}

public:
	void setName(std::string name)
	{
		this->name = std::move(name);
	}

	std::string_view getName() const
	{
		return name;
	}

	virtual void use() const = 0 ;

	virtual [[nodiscard]] bool drawUI() = 0;

};
