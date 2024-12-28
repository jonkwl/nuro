#include "diagnostics_window.h"
#include "window_includes.h"

#include <glm.hpp>
#include <implot.h>

#include "../core/diagnostics/profiler.h"
#include "../core/time/time.h"
#include "../core/diagnostics/diagnostics.h"

DiagnosticsWindow::DiagnosticsWindow() : fpsCache(std::deque<float>(100)),
fpsUpdateTimer(0.0f)
{
}

void DiagnosticsWindow::render()
{
	ImGui::Begin("Diagnostics", nullptr, EditorFlags::fixed);

	UIComponents::headline("Diagnostics", ICON_FA_MONITOR_WAVEFORM, HeadlineJustification::LEFT);

	UIComponents::indicatorLabel("Average FPS:", Diagnostics::getAverageFps());

	const int32_t values = 100;
	const float updateRate = 0.025f;
	int32_t maxValue = 1;
	if (fpsCache.size() > 0.0f)
		maxValue = *std::max_element(fpsCache.begin(), fpsCache.end());

	fpsUpdateTimer += Time::delta();
	if (fpsUpdateTimer >= updateRate)
	{
		fpsCache.push_back(Diagnostics::getFps());

		if (fpsCache.size() > values)
		{
			fpsCache.pop_front();
		}

		fpsUpdateTimer = 0.0f;
	}

	float* data = new float[values];
	for (size_t i = 0; i < fpsCache.size(); ++i)
	{
		data[i] = fpsCache[i];
	}

	ImVec4 low = ImVec4(1.0f, 0.0f, 0.5f, 1.0f);
	ImVec4 high = ImVec4(0.0f, 1.0f, 0.5f, 1.0f);
	ImVec4 color = UIUtils::lerpColors(low, high, glm::clamp((Diagnostics::getFps() - 0.0f) / (maxValue - 0.0f), 0.0f, 1.0f));
	ImVec2 size = ImVec2(120.0f, 40.0f);
	UIComponents::sparklineGraph("##spark", data, values, 0.0f, maxValue, 0.0f, color, size);

	delete[] data;

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("Current Draw Calls:", Diagnostics::getCurrentDrawCalls());
	UIComponents::indicatorLabel("Current Vertices:", Diagnostics::getCurrentVertices());
	UIComponents::indicatorLabel("Current Polygons:", Diagnostics::getCurrentPolygons());

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("CPU Entities:", Diagnostics::getNEntitiesCPU());
	UIComponents::indicatorLabel("GPU Entities:", Diagnostics::getNEntitiesGPU());

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("Render:", Profiler::get("render"), "ms");
	UIComponents::indicatorLabel("Shadow Pass:", Profiler::get("shadow_pass"), "ms");
	UIComponents::indicatorLabel("Pre Pass:", Profiler::get("pre_pass"), "ms");
	UIComponents::indicatorLabel("SSAO Pass:", Profiler::get("ssao"), "ms");
	UIComponents::indicatorLabel("Velocity Buffer Pass:", Profiler::get("velocity_buffer"), "ms");
	UIComponents::indicatorLabel("Forward Pass:", Profiler::get("forward_pass"), "ms");
	UIComponents::indicatorLabel("PP Pass:", Profiler::get("post_processing"), "ms");
	UIComponents::indicatorLabel("UI Pass:", Profiler::get("ui_pass"), "ms");

	ImGui::End();
}