#include "Profiler.h"
#include "glad/glad.h"

#include <imgui.h>
#include <chrono>
#include <array>
#include <vector>
#include <numeric>
#include <string>

const int frameSamples = 100;
std::array<float, frameSamples> frametimePlot;
unsigned int currentFrameIndex = 0;
float const longestFrame = 100.0f;

void profiler::recordFrame()
{
	static auto lastFrame = std::chrono::system_clock::now();
	auto currentFrame = std::chrono::system_clock::now();
	currentFrameIndex = (currentFrameIndex + 1) % frameSamples;
	frametime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFrame).count();
	frametimePlot[currentFrameIndex] = frametime;
	//longestFrame = std::max(longestFrame, frametime);
	fps = 1000.0f / frametime;
	lastFrame = currentFrame;
}

void profiler::drawUI(bool* open)
{
	if(!*open)
		return;
	float const plotHeight = 100;
	ImGui::Begin("Profiler", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Frametime: %.1f ms (%.1f) - (%.1f)", frametime, 0.0f, longestFrame);
	ImGui::PlotLines("###Frametimes", frametimePlot.data(), frameSamples, currentFrameIndex, nullptr, 0.0f, longestFrame, {ImGui::GetContentRegionAvailWidth(), plotHeight});
	ImGui::Text("FPS: %.1f", fps);

	struct GLContext
	{
		int majorVersion;
		int minorVersion;
		std::string vendor;
		std::string device;
		std::vector<std::string> supportedExtensions;
	};
	static GLContext const context = []() -> GLContext{
		GLContext ret;
		glGetIntegerv(GL_MAJOR_VERSION, &ret.majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &ret.minorVersion);
		ret.vendor = reinterpret_cast<char const*>(glGetString(GL_VENDOR));
		ret.device = reinterpret_cast<char const*>(glGetString(GL_RENDERER));
		int nSupportedExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &nSupportedExtensions);
		for(int i = 0; i < nSupportedExtensions; i++)
			ret.supportedExtensions.push_back(reinterpret_cast<char const*>(glGetStringi(GL_EXTENSIONS, i)));
		return ret;
	}();
	ImGui::Text("Context Info:");
	ImGui::Text("API: OpenGL %i.%i", context.majorVersion, context.minorVersion);
	ImGui::Text("Vendor: %s", context.vendor.data());
	ImGui::Text("Device: %s", context.device.data());
	if(ImGui::CollapsingHeader("Supported Extensions:"))
	{
		ImGui::Indent();
		ImGui::BeginChild("Supported Extensions", {0, 100});
		for(auto extension : context.supportedExtensions)
			ImGui::Text(extension.data());
		ImGui::EndChild();
		ImGui::Unindent();
	}
	ImGui::End();
}