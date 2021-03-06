﻿#include "ShaderManager.h"
#include "Camera.h"
#include "Lights.h"
#include "Globals.h"
#include "SceneManager.h"
#include "Node.h"
#include "Prop.h"
#include "Renderer.h"
#include "Profiler.h"
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW\glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <deque>
#include <thread>

double deltaTime = 0.0f;
double lastFrame = 0.0f;
double lastMouseX = 400;
double lastMouseY = 300;
bool mouseDrag = false;
Renderer& settings::mainRenderer()
{
	static Renderer r = {SceneManager::basic()->getAll<Camera>().front()};
	return r;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void* userParam);
void windowResizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int mode, int modifier);
void keyCallback(GLFWwindow* window, int key, int keycode, int mode, int modifier);
void processInput(GLFWwindow* window);
void drawUI();


int main(int argc, char** argv)
{
	//initialize stuff
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(info::windowWidth, info::windowHeight, "glSandbox", nullptr, nullptr);
	if(window == nullptr)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

#ifndef NDEBUG
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif
	glfwSetFramebufferSizeCallback(window, windowResizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
	glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGui::StyleColorsDark();
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::GetStyle().PopupRounding= 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	initializeResources();

	while(!glfwWindowShouldClose(window))
	{
		
		while(glfwPollEvents(), 
			!glfwGetWindowAttrib(window, GLFW_FOCUSED)
			|| info::windowHeight == 0 
			|| info::windowWidth == 0)
		{
			std::this_thread::yield();
		}
		processInput(window);
		drawUI();
		glfwSwapInterval(settings::rendering::vsync);
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		settings::mainRenderer().render();
		glDisable(GL_FRAMEBUFFER_SRGB);

		settings::postprocessing::steps()[0].draw(settings::mainRenderer().getOutput(), 0);
		glEnable(GL_FRAMEBUFFER_SRGB);
		//glDisable(GL_FRAMEBUFFER_SRGB);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		profiler::recordFrame();
	}

	glfwTerminate();
	return 0;
}

void drawUI()
{
	static bool drawFileBrowserFlag = false;
	static bool drawResources = false;
	static bool drawMainRenderer = false;
	static bool drawPostprocessingSettings = false;
	static bool drawProfiler = true;
	static bool drawImGuiDemo = false;
	static std::deque<bool> drawRenderer;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("View"))
		{
			if(ImGui::MenuItem("Resources"))
				drawResources = true;
			if(ImGui::BeginMenu("Rendering"))
			{
				if(ImGui::MenuItem("Main Renderer"))
					drawMainRenderer = true;
				for(int i = 0; i < settings::getAllRenderers().size(); i++)
					if(ImGui::MenuItem(("Secondary Renderer " + std::to_string(i)).data()))
						drawRenderer[i] = true;
				if(ImGui::MenuItem("Add Secondary Renderer"))
				{
					settings::addRenderer(std::make_unique<Renderer>());
					drawRenderer.push_back(true);
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Settings"))
			{
				if(ImGui::MenuItem("Postprocessing"))
					drawPostprocessingSettings = true;
				ImGui::EndMenu();
			}
			if(ImGui::MenuItem("Profiler"))
				drawProfiler = true;
			if(ImGui::BeginMenu("ImGui"))
			{
				if(ImGui::MenuItem("Demo"))
					drawImGuiDemo = true;
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	drawResourcesUI(&drawResources);
	settings::mainRenderer().drawUI(&drawMainRenderer);
	for(int i = 0; i < drawRenderer.size(); i++)
	{
		settings::getAllRenderers()[i]->drawUI(&drawRenderer[i]);
		if(drawRenderer[i])
			settings::getAllRenderers()[i]->render();
	}
	settings::postprocessing::drawUI(&drawPostprocessingSettings);
	profiler::drawUI(&drawProfiler);
	if(drawImGuiDemo)
		ImGui::ShowDemoWindow(&drawImGuiDemo);
}

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	info::windowWidth = width;
	info::windowHeight = height;

	glViewport(0, 0, width, height);
	settings::mainRenderer().resizeViewport(width, height);
	for(auto& step : settings::postprocessing::steps())
		step.updateFramebuffer();
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{

	static bool firstMouse = true;
	double xoffset = xpos - lastMouseX;
	double yoffset = ypos - lastMouseY;
	lastMouseX = xpos;
	lastMouseY = ypos;
	if(!mouseDrag || firstMouse)
	{
		firstMouse = false;
		return;
	}
	float sensitivity = 0.05f;
	xoffset *= -sensitivity;
	yoffset *= -sensitivity;
	settings::mainRenderer().getCamera()->rotate(xoffset, yoffset);
	settings::mainRenderer().shouldRender();
}
void mouseButtonCallback(GLFWwindow* window, int button, int mode, int modifier)
{
	if(ImGuiIO& io = ImGui::GetIO(); io.WantCaptureMouse)
		return ImGui_ImplGlfw_MouseButtonCallback(window, button, mode, modifier);

	if(button == GLFW_MOUSE_BUTTON_1)
	{
		if(mode == GLFW_PRESS)
		{
			mouseDrag = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			mouseDrag = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}
void keyCallback(GLFWwindow* window, int key, int keycode, int mode, int modifier)
{
	if(key == GLFW_KEY_ESCAPE && mode == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}
	return ImGui_ImplGlfw_KeyCallback(window, key, keycode, mode, modifier);
}
void processInput(GLFWwindow* window)
{

	float distance = 2.5f * deltaTime; // adjust accordingly
	glm::vec3 direction{0.0f};
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		direction.z += 1.0f;
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		direction.z -= 1.0f;
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		direction.x += 1.0f;
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		direction.x -= 1.0f;
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		direction.y += 1.0f;
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		direction.y -= 1.0f;
	if(direction != glm::vec3{0.0f})
	{
		settings::mainRenderer().getCamera()->move(direction * distance);
		settings::mainRenderer().shouldRender();
	}
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void* userParam)
{
	if(type == GL_DEBUG_TYPE_PERFORMANCE)
		return;
	std::cout << "-----------------------------------\n"
		<< "OpenGL Debug Message (" << id << "): \n" << message << '\n';
	std::cout << "Source: ";
	switch(source)
	{
		case GL_DEBUG_SOURCE_API:
			std::cout << "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			std::cout << "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			std::cout << "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			std::cout << "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			std::cout << "Application";
			break;
		default:
			std::cout << "Other";
			break;
	}
	std::cout << "\nType: ";
	switch(type)
	{
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "Deprecated Behaviour";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "Undefined Behaviour";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			std::cout << "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			std::cout << "Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			std::cout << "Pop Group";
			break;
		default:
			std::cout << "Other";
			break;
	}
	std::cout << "\nSeverity: ";
	switch(severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "High";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "Medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "Low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cout << "Notification";
			break;
	}
	std::cout << '\n';
}
