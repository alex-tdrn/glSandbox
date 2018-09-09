#pragma once

namespace profiler
{
	inline float frametime = 0.0f;
	inline float fps = 0.0f;
	void recordFrame();
	void drawUI(bool* open);
}