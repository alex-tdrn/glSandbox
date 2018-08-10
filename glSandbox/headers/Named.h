#pragma once
#include <string>

template <typename T>
class Name
{
private:
	inline static int ct{1};
	std::string name;

public:
	Name(std::string&& prefix)
		: name{prefix + '#' + std::to_string(ct++)}
	{
	}

public:
	std::string_view get() const
	{
		return name;
	}

	void set(std::string name)
	{
		this->name = name;
	}

};
