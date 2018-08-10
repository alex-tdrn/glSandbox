#pragma once
#include "Node.h"

#include <optional>

class Prop : public Node
{
private:
	Name<Prop> name{"prop"};
	std::optional<unsigned int> meshIndex;

public:
	Prop(std::optional<unsigned int> meshIndex = std::nullopt);
	
	virtual ~Prop() = default;

public:
	std::optional<unsigned int>const& getMeshIndex() const;

};