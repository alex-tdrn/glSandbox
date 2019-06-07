#include "Framebuffer.h"
#include "glad/glad.h"

Framebuffer::Framebuffer()
{
	glGenTextures(1, &colorbufferID);
	glGenRenderbuffers(1, &renderbufferID);

	update();

	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbufferID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferID);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &colorbufferID);
	glDeleteRenderbuffers(1, &renderbufferID);
	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::update()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorbufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::resize(glm::ivec2 size)
{
	if(this->size == size)
		return;
	this->size = size;
	update();
}

void Framebuffer::bind()
{
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	glViewport(0, 0, size.x, size.y);
}

unsigned int Framebuffer::getColorbufferID() const
{
	return colorbufferID;
}

void Framebuffer::drawUI() const
{
}
