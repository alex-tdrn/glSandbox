#include "Util.h"
#include "Globals.h"

#include <imgui.h>

class IDGuard
{
public:
	IDGuard() = delete;
	IDGuard(int* ID)
	{
		ImGui::PushID(ID);
	}
	IDGuard(char const* ID)
	{
		ImGui::PushID(ID);
	}
	IDGuard(void* ID)
	{
		ImGui::PushID(ID);
	}
	~IDGuard()
	{
		ImGui::PopID();
	}
};

inline void chooseGLEnumFromCombo(int& currentValue, std::vector<int> const& possibleValues)
{
	IDGuard idGuard{&currentValue};
	if(ImGui::BeginCombo("###Combo", glEnumToString(currentValue).data()))
	{
		int id = 0;
		for(int possibleValue : possibleValues)
		{
			ImGui::PushID(id++);
			bool isSelected = currentValue == possibleValue;
			if(ImGui::Selectable(glEnumToString(possibleValue).data(), &isSelected))
				currentValue = possibleValue;
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
}

inline void chooseComparisonFunctionFromCombo(int& currentValue)
{
	chooseGLEnumFromCombo(currentValue, {
		GL_ALWAYS, GL_NEVER, GL_LESS, GL_LEQUAL,
		GL_EQUAL, GL_NOTEQUAL, GL_GEQUAL, GL_GREATER
		});
}

template <typename T>
T* chooseFromCombo(T* currentValue, std::vector<std::unique_ptr<T>> const& possibleValues, bool nullable = false, std::string postfix = "")
{
	if(ImGui::BeginCombo(("###Combo" + postfix).data(), currentValue ? currentValue->getName().data() : "None"))
	{
		bool isSelected = !currentValue;
		if(nullable)
		{
			if(ImGui::Selectable("None", &isSelected))
				currentValue = nullptr;
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::Separator();
		}
		int id = 0;
		for(auto& v : possibleValues)
		{
			ImGui::PushID(id++);
			isSelected = currentValue == v.get();
			if(ImGui::Selectable(v->getName().data(), &isSelected))
				currentValue = v.get();
			if(isSelected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return currentValue;
}

inline void drawImage(unsigned int ID, int width, int height, bool flipX = false, bool flipY = false)
{
	float const size = std::min(ImGui::GetContentRegionAvailWidth(), 512.0f);
	ImVec2 uv1{flipX ? 1.0f : 0.0f, flipY ? 1.0f : 0.0f};
	ImVec2 uv2{1.0f - uv1.x, 1.0f - uv1.y};
	if(ImGui::ImageButton(ImTextureID(ID), ImVec2(size, size), uv1, uv2))
		ImGui::OpenPopup("###ResourceRenderer");
	if(ImGui::BeginPopupModal("###ResourceRenderer", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar))
	{
		float const percentOfScreen = 0.9f;
		glm::vec2 scale{width / info::windowWidth, height / info::windowHeight};
		glm::vec2 imageSize;
		if(scale.x > scale.y)//stretch by width
		{
			imageSize.x = info::windowWidth * percentOfScreen;
			imageSize.y = static_cast<float>(height) / width * imageSize.x;
		}
		else//stretch by height
		{
			imageSize.y = info::windowHeight * percentOfScreen;
			imageSize.x = static_cast<float>(width) / height * imageSize.y;
		}
		ImGui::Image(ImTextureID(ID), ImVec2(imageSize.x, imageSize.y), uv1, uv2);
		if(ImGui::IsAnyItemActive())
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

}

