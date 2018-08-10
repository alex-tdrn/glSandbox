#pragma once

class Renderer
{
private:
	unsigned int framebuffer;
	unsigned int colorbuffer;
	unsigned int renderbuffer;
	unsigned int simpleFramebuffer;
	unsigned int simpleColorbuffer;
	bool needRedraw = true;

public:
	void init();
	void render();
	void resizeFramebuffer();
	unsigned int getOutput();

};
inline Renderer renderer;