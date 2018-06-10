#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"
#include "Util.h"

#include <stb_image.h>
#include <imgui.h>
#include <thread>
#include <queue>
#include <mutex>
#include <array>

class Loader
{
public:
	struct Job
	{
		std::string path;
		unsigned char** data;
		int* width;
		int* height;
		int* nrChannels;
		std::atomic<bool>* readyFlag;
	};

private:
	std::atomic<bool> finish = false;
	std::array<std::thread, 5> workers;
	std::queue<Job> jobs;
	std::mutex queueMutex;

public:
	Loader()
	{
		for(auto& worker : workers)
			worker = std::thread(&Loader::run, this);
	}
	~Loader()
	{
		finish = true;
		for(auto& worker : workers)
			if(worker.joinable())
				worker.join();
	}

private:
	void run()
	{
		while(!finish)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(10ms);
			Job currentJob;
			queueMutex.lock();
			if(!jobs.empty())
			{
				currentJob = jobs.front();
				jobs.pop();
				queueMutex.unlock();
				doJob(currentJob);
				continue;
			}
			queueMutex.unlock();
		}
	}

	void doJob(Job& job)
	{
		*job.data = stbi_load(job.path.data(), job.width, job.height, job.nrChannels, STBI_default);
		if(job.data)
			*job.readyFlag = true;
		else
			throw ("Could not load texture" + job.path);
	}
	
public:
	void addJob(Job newJob)
	{
		queueMutex.lock();
		jobs.push(newJob);
		queueMutex.unlock();
	}

};

Loader textureLoader;

Texture::Texture(Texture const& other)
	: path(std::move(other.path)), ID(other.ID), initialized(other.initialized), ready(other.ready), location(other.location),
	linear(other.linear)
{
	image = other.image;
}

Texture::Texture(std::string const path, int location, bool linear)
	:path(path), location(location), linear(linear)
{
	image = new ImageData();
	textureLoader.addJob({path, &(image->data), &(image->width), &(image->height), &(image->nrChannels), ready});

}

Texture::~Texture()
{
	//glDeleteTextures(1, &ID);
	if(image)
	{
		/*delete image;
		if(image->data)
			delete image->data;*/
	}
}

Texture const& Texture::operator=(Texture const& other)
{
	this->initialized = other.initialized;
	this->linear = other.linear;
	this->location = other.location;
	this->path = other.path;
	this->ID = other.ID;
	*(this->image) = *other.image;
	return *this;
}

glm::vec2 const Texture::getUVOffset() const
{
	return {-uvOffset.x, uvOffset.y};
}

bool Texture::isInitialized() const
{
	if(!initialized && *ready)
	{
		initialized = true;
		
		GLenum format;
		GLenum pixelTransfer;
		switch(image->nrChannels)
		{
			case 1:
				format = GL_R8;
				pixelTransfer = GL_RED;
				break;
			case 2:
				format = GL_RG8;
				pixelTransfer = GL_RG;
				break;
			case 3:
				format = GL_RGB8;
				pixelTransfer = GL_RGB;
				break;
			case 4:
				format = GL_RGBA8;
				pixelTransfer = GL_RGBA;
				break;
		}
		if(!linear)
		{
			switch(image->nrChannels)
			{
				case 3:
					format = GL_SRGB8;
					break;
				case 4:
					format = GL_SRGB8_ALPHA8;
					break;
			}
		}
		glActiveTexture(GL_TEXTURE0 + location);
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, image->width, image->height, 0, pixelTransfer, GL_UNSIGNED_BYTE, image->data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image->data);
		resources::scenes::getActiveScene().update();
	}
	return initialized;
}

std::string_view Texture::getPath() const
{
	return path;
}

int Texture::getLocation() const
{
	return location;
}

void Texture::use() const
{
	if(!isInitialized())
		return;
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

bool Texture::drawUI()
{
	bool valueChanged = false;
	if(!initialized)
	{
		ImGui::Text("Loading...");
		return false;
	}
	ImGui::PushID(this);
	if(drag2("UV Offset", 0.0001f, uvOffset.x, uvOffset.y, 0.0f, 1.0f))
		valueChanged = true;
	if(ImGui::SliderFloat("U ", &uvOffset.x, 0.0f, 1.0f))
		valueChanged = true;
	if(ImGui::SliderFloat("V ", &uvOffset.y, 0.0f, 1.0f))
		valueChanged = true;
	float const size = ImGui::GetColumnWidth();
	ImGui::Text("%i x %i", image->width, image->height);
	ImGui::Text("# channels %i", image->nrChannels);
	ImGui::Text("Path: %s", path.data());
	ImGui::Image(ImTextureID(ID), ImVec2(size, size), ImVec2(-uvOffset.x, uvOffset.y), ImVec2(-uvOffset.x + 1.0f, uvOffset.y + 1.0f));
	if(false && size < 256 && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image(ImTextureID(ID), ImVec2(512, 512));
		ImGui::EndTooltip();
	}
	ImGui::PopID();
	return valueChanged;
}