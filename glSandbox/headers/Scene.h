#pragma once
#include "Actor.h"
#include "Lights.h"
#include "Camera.h"
#include "Cubemap.h"

#include <vector>

class Scene
{
private:
	std::string name;
	unsigned int framebuffer;
	unsigned int colorbuffer;
	unsigned int renderbuffer;
	bool initialized = false;
	bool needRedraw = true;
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	std::vector<Actor> actors;
	Camera camera;
	glm::vec3 backgroundColor{0.5f, 0.5f, 0.5f};
	Cubemap* skybox = nullptr;
	inline static int ct = 0;

public:
	Scene(std::string const name = "Model#" + std::to_string(ct++));

private:
	void init();

public:
	std::string_view const getName();
	void add(DirectionalLight light);
	void add(PointLight light);
	void add(SpotLight light);
	void add(Actor actor);
	void update();
	void updateFramebuffer();
	void draw();
	void setBackgroundColor(glm::vec3 color);
	unsigned int getColorbuffer() const;
	Camera& getCamera();

	void drawUI();
};