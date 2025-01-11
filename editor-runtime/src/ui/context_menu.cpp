#include "context_menu.h"

#include <imgui.h>

#include "../src/ui/editor_ui.h"

namespace ContextMenu {

	void _space() {
		ImGui::Dummy(ImVec2(0.0f, 0.1f));
	}

	bool begin()
	{
		// Custom context menu colors
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.25f, 0.8f));		// Popup background
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));			// Text
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.15f, 1.0f));	// Hover
		ImGui::PushStyleColor(ImGuiCol_Border, EditorColor::selection);					// Outline

		return ImGui::BeginPopupContextWindow(EditorUI::generateId().c_str(), ImGuiPopupFlags_MouseButtonRight);
	}

	void end()
	{
		ImGui::EndPopup();
	}

	void pop()
	{
		ImGui::PopStyleColor(4);
	}

	bool item(const char* icon, std::string title)
	{
		_space();
		std::string text = std::string(icon) + "     " + title;
		return ImGui::MenuItem(text.c_str());
	}

	bool itemLight(std::string title)
	{
		return ImGui::MenuItem(title.c_str());
	}

	bool menu(const char* icon, std::string title)
	{
		_space();
		std::string text = std::string(icon) + "     " + title;
		return ImGui::BeginMenu(text.c_str());
	}

	void endMenu()
	{
		ImGui::EndMenu();
	}

	void seperator()
	{
		_space();
		ImGui::Separator();
	}

}