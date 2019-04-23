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
T* chooseFromCombo(T* currentValue, std::vector<std::unique_ptr<T>> const& possibleValues, bool nullable = false, std::string prefix = "")
{
	if(ImGui::BeginCombo(("###Combo" + prefix).data(), currentValue ? currentValue->getName().data() : "None"))
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

namespace ImGui
{
	template <typename MemberFunction, typename Class>
	void Checkbox(const char* label, bool flag, 
		MemberFunction setFlag, Class* instance)
	{
		if(ImGui::Checkbox(label, &flag))
			(instance->*setFlag)(flag);
	}

	template <typename MemberFunction, typename Class>
	void Checkbox(const char* label, bool flag, 
		MemberFunction enableFlag, MemberFunction disableFlag, Class* instance)
	{
		if(ImGui::Checkbox(label, &flag))
		{
			if(flag)
				(instance->*enableFlag)();
			else
				(instance->*disableFlag)();
		}
	}
	
	template <typename MemberFunction, typename Class>
	void ColorEdit3(const char* label, glm::vec3 color, ImGuiColorEditFlags flags, 
		MemberFunction setColor, Class* instance)
	{
		if(ImGui::ColorEdit3(label, &color.r, flags))
			(instance->*setColor)(color);
	}

	template <typename MemberFunction, typename Class>
	void ColorEdit4(const char* label, glm::vec4 color, ImGuiColorEditFlags flags,
		MemberFunction setColor, Class* instance)
	{
		if(ImGui::ColorEdit4(label, &color.r, flags))
			(instance->*setColor)(color);
	}

	template <typename MemberFunction, typename Class>
	void SliderFloat(const char* label, float value, float min, float max,
		MemberFunction setValue, Class* instance)
	{
		if(ImGui::SliderFloat(label, &value, min, max))
			(instance->*setValue)(value);
	}

	template <typename T, typename MemberFunction, typename Class>
	void ChooseFromCombo(const char* label, T* currentValue, std::vector<std::unique_ptr<T>> const& possibleValues, bool nullable, 
		MemberFunction setValue, Class* instance)
	{
		if(ImGui::BeginCombo((std::string("###Combo") + label).data(), currentValue ? currentValue->getName().data() : "None"))
		{
			bool isSelected = !currentValue;
			if(nullable)
			{
				if(ImGui::Selectable("None", &isSelected))
				{
					currentValue = nullptr;
					(instance->*setValue)(currentValue);
				}
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
				{
					currentValue = v.get();
					(instance->*setValue)(currentValue);
				}
				if(isSelected)
					ImGui::SetItemDefaultFocus();
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}
	}
}