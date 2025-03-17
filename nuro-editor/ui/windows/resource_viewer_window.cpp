#include "resource_viewer_window.h"

#include <glm/glm.hpp>
#include <implot.h>

#include <time/time.h>
#include <utils/ioutils.h>
#include <diagnostics/profiler.h>
#include <diagnostics/diagnostics.h>

ResourceViewerWindow::ResourceViewerWindow()
{
}

void ResourceViewerWindow::render()
{
	std::string title = UIUtils::windowTitle("Resource Viewer");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::fixed);
	{
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		IMComponents::headline("Resource Viewer", ICON_FA_DATABASE);

		const auto& resources = ApplicationContext::resourceManager().readResources();

		IMComponents::label("Resources allocated: ", EditorUI::getFonts().h4);
		ImGui::SameLine();
		IMComponents::label(std::to_string(resources.size()), EditorUI::getFonts().h4_bold);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		
		// TEMPORARY!
		// Inefficient, for debugging
		
		ImGui::BeginChild(EditorUI::generateId(), ImVec2(ImGui::GetContentRegionAvail()));
		{
			for (const auto& [id, resource] : resources) {
				IMComponents::label("ID " + std::to_string(id) + ": ", EditorUI::getFonts().p_bold);
				ImGui::SameLine();
				ImVec2 pos = ImGui::GetCursorScreenPos();
				switch (resource->resourceState()) {
				case ResourceState::EMPTY:
					IMComponents::label("EMPTY", IM_COL32(255, 255, 0, 255));
					ImGui::SameLine();
					break;
				case ResourceState::QUEUED:
					IMComponents::label("QUEUED", IM_COL32(180, 180, 255, 255));
					ImGui::SameLine();
					break;
				case ResourceState::LOADING:
					IMComponents::label("CREATING...", IM_COL32(100, 255, 145, 255));
					ImGui::SetCursorScreenPos(pos);
					IMComponents::loadingBuffer(drawList, pos + ImVec2(74.0f, 3.0f), 5.0f, 2.0f, IM_COL32(255, 255, 255, 200));
					ImGui::SetCursorScreenPos(pos + ImVec2(87.0f, 0.0f));
					break;
				case ResourceState::READY:
					IMComponents::label("READY", IM_COL32(0, 255, 0, 255));
					ImGui::SameLine();
					break;
				case ResourceState::FAILED:
					IMComponents::label("FAILED", IM_COL32(255, 50, 50, 255));
					ImGui::SameLine();
					break;
				}
				IMComponents::label(" (" + IOUtils::getFilename(resource->sourcePath()) + ") ");
				ImGui::SameLine();
				IMComponents::flagLabel("- Allocated in RAM", resource->loadedIoData());
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}