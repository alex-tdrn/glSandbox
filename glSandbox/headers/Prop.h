#pragma once
#include "Node.h"
#include "Util.h"

#include <optional>

class Prop : public Node
{
private:
	bool enabled = true;
	bool outlined = false;
	//TODO^^^
	std::optional<size_t> meshIndex;

public:
	Name<Prop> name{"prop"};

public:
	Prop(std::optional<size_t> meshIndex = std::nullopt);
	virtual ~Prop() = default;

public:
	std::optional<size_t>const& getMeshIndex() const;
	Bounds getBounds() const override;
	//void drawUI() override;

};