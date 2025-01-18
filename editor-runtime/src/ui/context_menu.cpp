#include "context_menu.h"

#include <imgui.h>

#include "../src/ui/editor_ui.h"

namespace ContextMenu {

	ImVec4 _backgroundColor = ImVec4(0.2f, 0.2f, 0.25f, 0.9f);
	ImVec4 _textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 _hoverColor = ImVec4(0.1f, 0.1f, 0.15f, 1.0f);
	ImU32 _outlineColor = EditorColor::selection;

	void _space() {
		ImGui::Dummy(ImVec2(0.0f, 0.1f));
	}

	bool begin()
	{
		// Custom context menu colors
		ImGui::PushStyleColor(ImGuiCol_PopupBg, _backgroundColor);
		ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, _hoverColor);
		ImGui::PushStyleColor(ImGuiCol_Border, _outlineColor);

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