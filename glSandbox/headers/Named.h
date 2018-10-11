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
	Name() = default;
	Name(Name const&) = delete;
	Name(Name&&) = default;
	Name& operator=(Name const&) = delete;
	Name& operator=(Name&&) = default;
	~Name() = default;
public:
	std::string const& get() const
	{
		return name;
	}

	void set(std::string name)
	{
		this->name = std::move(name);
	}

};
