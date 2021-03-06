#pragma once
#include <string_view>
#include <string>

template <typename T>
class AutoName
{
private:
	mutable bool initialized = false;
	inline static int counter{1};
	mutable std::string name;

protected:
	AutoName() = default;
	AutoName(AutoName const&) = default;
	AutoName(AutoName&&) = default;
	AutoName& operator=(AutoName const&) = default;
	AutoName& operator=(AutoName&&) = default;
	~AutoName() = default;

protected:
	virtual std::string getNamePrefix() const = 0;

public:
	std::string const& getName() const
	{
		if(!initialized)
		{
			initialized = true;
			name = getNamePrefix() + "(#" + std::to_string(counter++) + ")";
		}
		return name;
	}

	void setName(std::string name)
	{
		initialized = true;
		this->name = std::move(name);
	}

};
