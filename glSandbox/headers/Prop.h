#pragma once
#include "Node.h"

#include <optional>

class Prop : public Node
{
private:
	std::optional<unsigned int> meshIndex;

public:
	Name<Prop> name{"prop"};

public:
	Prop(std::optional<unsigned int> meshIndex = std::nullopt);
	virtual ~Prop() = default;

public:
	std::optional<unsigned int>const& getMeshIndex() const;
	//void drawUI() override;

};