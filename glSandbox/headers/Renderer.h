#pragma once
#include "AutoName.h"
#include "Producer.h"
#include "Framebuffer.h"
#include <memory>

class Renderer : public AutoName<Renderer>, public Producer<Framebuffer>
{
private:
	glm::ivec2 size = { 1920, 1080 };
	glm::vec3 background = { 0.0f, 0.015f, 0.015f };

protected:
	mutable std::unique_ptr<Framebuffer> framebuffer = std::make_unique<Framebuffer>();

public:
	Renderer() = default;
	virtual ~Renderer() = default;

private:
	virtual std::string getNamePrefix() const override;
	Framebuffer const* getResourceHandle() const final override;

protected:
	virtual void update() const override;

public:
	virtual void drawUI(bool* open);

};