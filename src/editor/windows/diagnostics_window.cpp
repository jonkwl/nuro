#include "diagnostics_window.h"

#include "implot.h"

#include "../src/editor/ui_components.h"
#include "../src/editor/IconsFontAwesome6.h"
#include "../src/runtime/runtime.h"
#include "../src/editor/editor_ui.h"
#include "../src/utils/profiler.h"

std::deque<float> DiagnosticsWindow::fpsCache = std::deque<float>(100);
float DiagnosticsWindow::fpsUpdateTimer = 0.0f;

ImVec4 lerpColors(const ImVec4& a, const ImVec4& b, float t)
{
	return ImVec4(
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z),
		a.w + t * (b.w - a.w));
}

void sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size)
{
	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
	if (ImPlot::BeginPlot(id, size, ImPlotFlags_CanvasOnly))
	{
		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
		ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
		ImPlot::SetNextLineStyle(col);
		ImPlot::SetNextFillStyle(col, 0.25);
		ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleVar();
}

float remap(float old_min, float old_max, float new_min, float new_max, float value)
{
	value = glm::clamp(value, old_min, old_max);
	return new_min + (value - old_min) * (new_max - new_min) / (old_max - old_min);
}

void DiagnosticsWindow::prepare()
{
	ImGui::Begin("Diagnostics", nullptr, EditorUI::windowFlags.fixed);

	UIComponents::headline("Diagnostics", ICON_FA_MONITOR_WAVEFORM, HeadlineJustification::LEFT);

	UIComponents::indicatorLabel("Average FPS:", Runtime::averageFps);

	const int values = 100;
	const float updateRate = 0.025f;
	int maxValue = 1;
	if (fpsCache.size() > 0.0f)
		maxValue = *std::max_element(fpsCache.begin(), fpsCache.end());

	fpsUpdateTimer += Runtime::deltaTime;
	if (fpsUpdateTimer >= updateRate)
	{
		fpsCache.push_back(Runtime::fps);

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
	ImVec4 color = lerpColors(low, high, remap(0.0f, maxValue, 0.0f, 1.0f, Runtime::fps));
	sparkline("##spark", data, values, 0.0f, maxValue, 0.0f, color, ImVec2(120.0f, 40.0f));

	delete[] data;

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("Current Draw Calls:", Runtime::currentDrawCalls);
	UIComponents::indicatorLabel("Current Vertices:", Runtime::currentVertices);
	UIComponents::indicatorLabel("Current Polygons:", Runtime::currentPolygons);

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("CPU Entities:", Runtime::nCPUEntities);
	UIComponents::indicatorLabel("GPU Entities:", Runtime::nGPUEntities);

	UIComponents::space(0.0f, 5.0f);

	UIComponents::indicatorLabel("Render:", Profiler::get("render"), "ms");
	UIComponents::indicatorLabel("Shadow Pass:", Profiler::get("shadow_pass"), "ms");
	UIComponents::indicatorLabel("Forward Pass:", Profiler::get("forward_pass"), "ms");
	UIComponents::indicatorLabel("PP Pass:", Profiler::get("post_processing"), "ms");
	UIComponents::indicatorLabel("UI Pass:", Profiler::get("ui_pass"), "ms");

	ImGui::End();
}