#pragma once
#include "AutoName.h"
#include "Producer.h"
#include "Framebuffer.h"
#include <memory>

class Renderer : public Producer<Framebuffer>, public AutoName<Renderer>
{
protected:
	mutable std::unique_ptr<Framebuffer> framebuffer;

public:
	Renderer() = default;
	virtual ~Renderer() = default;

private:
	virtual std::string getNamePrefix() const override;
	Framebuffer const* getResourceHandle() const final override;

};