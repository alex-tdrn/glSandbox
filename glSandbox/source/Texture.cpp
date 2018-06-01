#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include "Globals.h"

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
	: path(std::move(other.path)), ID(other.ID), initialized(other.initialized), ready(other.ready)
{
	image = other.image;
}

Texture::Texture(std::string const path)
	:path(path)
{
	image = new ImageData();
	textureLoader.addJob({path, &(image->data), &(image->width), &(image->height), &(image->nrChannels), ready});

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
	this->path = other.path;
	this->ID = other.ID;
	*(this->image) = *other.image;
	return *this;
}
bool Texture::isInitialized() const
{
	if(!initialized && *ready)
	{
		initialized = true;
		glBindTexture(GL_TEXTURE_2D, ID);
		if(image->nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image->data);
	}
	return initialized;
}

std::string_view Texture::getPath() const
{
	return path;
}

void Texture::use(GLenum location) const
{
	if(!*ready)
		return;
	if(!initialized)
	{
		glBindTexture(GL_TEXTURE_2D, ID);
		if(image->nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image->data);
		initialized = true;
		resources::scene.update();
	}
	glActiveTexture(location);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::drawUI() const
{
	if(!initialized)
	{
		ImGui::Text("Loading...");
		return;
	}
	float const size = ImGui::GetColumnWidth();
	ImGui::Text("%i x %i", image->width, image->height);
	ImGui::Text("# channels %i", image->nrChannels);
	ImGui::Text("Path: %s", path.data());
	ImGui::Image(ImTextureID(ID), ImVec2(size, size));
	if(false && size < 256 && ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Image(ImTextureID(ID), ImVec2(512, 512));
		ImGui::EndTooltip();
	}
}