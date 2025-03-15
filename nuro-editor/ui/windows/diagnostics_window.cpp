#include "diagnostics_window.h"

#include <glm/glm.hpp>
#include <implot.h>

#include <time/time.h>
#include <diagnostics/profiler.h>
#include <diagnostics/diagnostics.h>

DiagnosticsWindow::DiagnosticsWindow() : fpsCache(std::deque<float>(100)),
fpsUpdateTimer(0.0f)
{
}

void DiagnosticsWindow::render()
{
	// Get fps
	// float fps = 1000000.0f / Profiler::getUs("render");
	float fps = Diagnostics::getFps();

	std::string title = UIUtils::windowTitle("Diagnostics");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::fixed);
	{
		IMComponents::headline("Diagnostics", ICON_FA_MONITOR_WAVEFORM);

		IMComponents::indicatorLabel("Average FPS:", Diagnostics::getAverageFps());

		const int32_t values = 100;
		const float updateRate = 0.025f;
		int32_t maxValue = 1;
		if (fpsCache.size() > 0.0f) {
			maxValue = static_cast<uint32_t>(*std::max_element(fpsCache.begin(), fpsCache.end()));
		}

		fpsUpdateTimer += Time::deltaf();
		if (fpsUpdateTimer >= updateRate)
		{

			// Reconstruct fps of game view (tmp, only showing reconstructed fps of game view rendering atm)
			fpsCache.push_back(fps);

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
		// ImVec4 color = UIUtils::lerpColors(low, high, glm::clamp((Diagnostics::getFps() - 0.0f) / (maxValue - 0.0f), 0.0f, 1.0f));
		ImVec4 color = high;
		ImVec2 size = ImVec2(120.0f, 40.0f);
		IMComponents::sparklineGraph("##spark", data, values, 0.0f, maxValue, 0.0f, color, size);

		delete[] data;

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		IMComponents::indicatorLabel("Current Draw Calls:", Diagnostics::getCurrentDrawCalls());
		IMComponents::indicatorLabel("Current Vertices:", Diagnostics::getCurrentVertices());
		IMComponents::indicatorLabel("Current Polygons:", Diagnostics::getCurrentPolygons());

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		IMComponents::indicatorLabel("CPU Entities:", Diagnostics::getNEntitiesCPU());
		IMComponents::indicatorLabel("GPU Entities:", Diagnostics::getNEntitiesGPU());

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		IMComponents::indicatorLabel("Rendering:", Profiler::getMs("render"), "ms");
		IMComponents::indicatorLabel("Physics:", Profiler::getMs("physics"), "ms");
		IMComponents::indicatorLabel("Shadow Pass:", Profiler::getMs("shadow_pass"), "ms");
		IMComponents::indicatorLabel("Preprocessor Pass:", Profiler::getUs("preprocessor_pass"), "ns");
		IMComponents::indicatorLabel("Pre Pass:", Profiler::getMs("pre_pass"), "ms");
		IMComponents::indicatorLabel("SSAO Pass:", Profiler::getMs("ssao"), "ms");
		IMComponents::indicatorLabel("Velocity Buffer Pass:", Profiler::getMs("velocity_buffer"), "ms");
		IMComponents::indicatorLabel("Forward Pass:", Profiler::getMs("forward_pass"), "ms");
		IMComponents::indicatorLabel("PP Pass:", Profiler::getMs("post_processing"), "ms");
		IMComponents::indicatorLabel("UI Pass:", Profiler::getMs("ui_pass"), "ms");
		IMComponents::indicatorLabel("Scene View:", Profiler::getMs("scene_view"), "ms");
	}
	ImGui::End();
}