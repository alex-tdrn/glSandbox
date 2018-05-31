#include "Model.h"
#include "Actor.h"
#include "Shader.h"
#include "Camera.h"
#include "Lights.h"
#include "Globals.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <optional>

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = 400;
float lastMouseY = 300;
bool mouseDrag = false;

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

	//setup ImGui
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfwGL3_Init(window, false);
	ImGui::StyleColorsLight();
	
	resources::scene.setBackgroundColor({1.0f, 1.0f, 1.0f});
	resources::scene.getCamera().setPosition(Position{{3.0f, 2.0f, 1.0f}});
	resources::scene.getCamera().setOrientation(Orientation{200.0f, -20.0f, 0.0f});
	resources::init();
	{
		DirectionalLight light;
		light.setColor({20.0f / 255.0f, 10.0f / 255.0f, 70.0f / 255.0f});
		light.setOrientation({120.0f, -60.0f, 0.0f});
		resources::scene.add(light);
	}
	{
		PointLight light;
		light.setColor({1.0f, 200.0f / 255.0f, 0.0f});
		light.setPosition({-1.0f, 0.0f, 2.0f});
		resources::scene.add(light);
	}
	{
		SpotLight light;
		light.setPosition({-2.4f, -1.4f, -2.4f});
		light.setOrientation({170.0f, 0.0f, 0.0f});
		resources::scene.add(light);
	}
	{
		Actor actor(&resources::models::sponza);
		actor.setPosition({0.0f, -1.8f, 0.0f});
		actor.setScale({0.0125f, 0.0125f, 0.0125f});
#ifdef NDEBUG
		resources::scene.add(actor);
#endif
	}
	{
		int idx = 1;
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				Actor actor(&resources::models::nanosuit);
				actor.setPosition({0.0f - i * 3.0f, -1.75f, 0.0f - j * 1.0f});
				actor.setScale({0.15f, 0.15f, 0.15f});
				resources::scene.add(actor);
				idx++;
			}
		}
	}
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		processInput(window);
		drawUI();
		glfwSwapInterval(settings::rendering::vsync);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		resources::scene.draw();

		{
			using settings::postprocessing::steps;
			if(steps.size() == 1)
			{
				steps[0].draw(resources::scene.getColorbuffer(), 0);
			}
			else if(steps.size() > 1)
			{
				steps[0].draw(resources::scene.getColorbuffer());
				for(int i = 1; i < steps.size() - 1; i++)
					steps[i].draw(steps[i - 1].getColorbuffer());
				steps[steps.size() - 1].draw(steps[steps.size() - 2].getColorbuffer(), 0);
			}

		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void drawUI()
{
	ImGui_ImplGlfwGL3_NewFrame();
	resources::drawUI();
	settings::drawUI();
	info::drawUI();

}
void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	info::windowWidth = width;
	info::windowHeight = height;
	glViewport(0, 0, width, height);
	resources::scene.updateFramebuffer();
	for(auto& step : settings::postprocessing::steps)
		step.updateFramebuffer();
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{

	static bool firstMouse = true;
	float xoffset = xpos - lastMouseX;
	float yoffset = ypos - lastMouseY;
	lastMouseX = xpos;
	lastMouseY = ypos;
	if(!mouseDrag || firstMouse)
	{
		firstMouse = false;
		return;
	}
	resources::scene.update();

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= -sensitivity;
	resources::scene.getCamera().adjustOrientation(xoffset, yoffset);
}
void mouseButtonCallback(GLFWwindow* window, int button, int mode, int modifier)
{
	if(ImGuiIO& io = ImGui::GetIO(); io.WantCaptureMouse)
		return ImGui_ImplGlfw_MouseButtonCallback(window, button, mode, modifier);
	resources::scene.update();


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
	if(ImGuiIO& io = ImGui::GetIO(); io.WantCaptureKeyboard)
		return ImGui_ImplGlfw_KeyCallback(window, key, keycode, mode, modifier);
	if(mode != GLFW_PRESS)
		return;
	resources::scene.update();

	switch(key)
	{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
	}
}
void processInput(GLFWwindow* window)
{

	float moveDistance = 2.5f * deltaTime; // adjust accordingly
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		resources::scene.getCamera().dolly(+moveDistance);
		resources::scene.update();
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		resources::scene.getCamera().dolly(-moveDistance);
		resources::scene.update();
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		resources::scene.getCamera().pan({+moveDistance, 0.0f});
		resources::scene.update();
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		resources::scene.getCamera().pan({-moveDistance, 0.0f});
		resources::scene.update();
	}
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		resources::scene.getCamera().pan({0.0f, +moveDistance});
		resources::scene.update();
	}
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		resources::scene.getCamera().pan({0.0f, -moveDistance});
		resources::scene.update();
	}
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void* userParam)
{
	std::cout << "-----------------------------------\n"
		<< "OpenGL Debug Message (" << id << "): " << message << '\n';
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
