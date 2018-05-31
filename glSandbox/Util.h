#pragma once
#include <imgui.h>

inline void wrapAround(float& x, float const min, float const max)
{
	float const diff = max - min;
	while(x < min)
		x += diff;
	while(x > max)
		x -= diff;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y)
{
	bool valueChanged = false;
	ImGui::Button(title);
	if(ImGui::IsItemActive())
	{
		ImVec2 mouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);
		x += mouseDelta.x  * sensitivity;
		y -= mouseDelta.y * sensitivity;
		if(mouseDelta.x != 0 || mouseDelta.y != 0)
			valueChanged = true;
	}
	return valueChanged;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y, float const min, float const max)
{
	bool ret = drag2(title, sensitivity, x, y);
	wrapAround(x, min, max);
	wrapAround(y, min, max);
	return ret;
}

inline bool drag2(const char* title, float const sensitivity, float& x, float& y, float const xmin, float const xmax, const float ymin, const float ymax)
{
	bool ret = drag2(title, sensitivity, x, y);
	wrapAround(x, xmin, xmax);
	wrapAround(y, ymin, ymax);
	return ret;
}
