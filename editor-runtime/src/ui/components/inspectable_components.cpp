#include "inspectable_components.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <string>

#include "../core/rendering/icons/icon_pool.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/collection/IconsFontAwesome6.h"

namespace InspectableComponents {

	bool _beginComponent(std::string label, uint32_t icon, bool& opened, bool* enabledPtr)
	{
		//
		// EVALUATE 
		//

		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		ImVec2 contentAvail = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float titleHeight = EditorSizing::h4_FontSize;
		ImVec2 titlePadding = ImVec2(10.0f, 10.0f);

		float collapsedHeight = titleHeight + titlePadding.y * 2;
		float expandedHeight = 250.0f;

		float yMargin = 2.0f;
		ImVec2 size = ImVec2(contentAvail.x, opened ? expandedHeight : collapsedHeight);
		ImVec2 p0 = ImVec2(cursorPos.x, cursorPos.y + yMargin);
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

		const bool hovered = ImGui::IsMouseHoveringRect(p0, p1);

		ImVec2 cursor = p0 + titlePadding;

		//
		// CLICK COLLAPSE
		//

		if (hovered && ImGui::IsMouseClicked(2)) {
			opened = !opened;
		}

		//
		// DRAW BACKGROUND
		//

		drawList.AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 255), 10.0f);

		//
		// DRAW EXPANSION CARET
		//

		if (IMComponents::caret(drawList, cursor, ImVec2(-1.0f, 2.0f), opened ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT, IM_COL32(0, 0, 0, 0), EditorColor::element)) {
			opened = !opened;
		}
		cursor.x += 22.0f;

		//
		// DRAW COMPONENT ICON
		//

		ImVec2 iconSize = ImVec2(18.0f, 18.0f);
		drawList.AddImage(icon, cursor, cursor + iconSize, ImVec2(0, 1), ImVec2(1, 0));
		cursor.x += iconSize.x + 8.0f;

		//
		// DRAW COMPONENT CHECKBOX
		//

		if (enabledPtr) {

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));

			ImVec2 internalCursor = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(cursor + ImVec2(0.0f, -2.0f));
			std::string id = EditorUI::generateIdString();
			ImGui::Checkbox(id.c_str(), enabledPtr);
			ImGui::SetCursorScreenPos(internalCursor);
			cursor.x += 28.0f;

			ImGui::PopStyleVar();
		}

		//
		// DRAW COMPONENT TEXT
		//

		drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, cursor, EditorColor::text, label.c_str());

		//
		// ADVANCE CURSOR AND DRAW CUSTOM CONTENT
		//

		if (opened) {
			ImGui::Dummy(ImVec2(0.0f, collapsedHeight + titlePadding.y));
		}
		else {
			ImGui::Dummy(ImVec2(size.x, size.y + yMargin));
		}

		return opened;
	}

	void _headline(std::string label) {
		IMComponents::label(label, EditorUI::getFonts().p_bold);
		ImGui::Dummy(ImVec2(0.0f, 1.0f));
	}

	void _spacing() {
		ImGui::Dummy(ImVec2(0.0f, 6.0f));
	}

	void drawTransform(TransformComponent& transform)
	{
		static bool opened = false;
		if (_beginComponent("Transform", IconPool::get("transform"), opened, nullptr))
		{
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();
			glm::vec3 rotationTmp = glm::degrees(glm::eulerAngles(transform.rotation));
			IMComponents::input("Position", transform.position);
			IMComponents::input("Rotation", rotationTmp);
			IMComponents::input("Scale", transform.scale);
		}
	}

	void drawMeshRenderer(MeshRendererComponent& meshRenderer)
	{
		static bool opened = false;
		if (_beginComponent("Mesh Renderer", IconPool::get("mesh_renderer"), opened, &meshRenderer.enabled))
		{
		}
	}

	void drawCamera(CameraComponent& camera)
	{
		static bool opened = false;
		if (_beginComponent("Camera", IconPool::get("camera"), opened, &camera.enabled))
		{
			_headline("General");
			IMComponents::input("FOV", camera.fov);
			IMComponents::input("Near", camera.near);
			IMComponents::input("Far", camera.far);
		}
	}

	void drawDirectionalLight(DirectionalLightComponent& directionalLight)
	{
		static bool opened = false;
		if (_beginComponent("Directional Light", IconPool::get("directional_light"), opened, &directionalLight.enabled))
		{
			IMComponents::input("Intensity", directionalLight.intensity);
			IMComponents::colorPicker("Color", directionalLight.color);
		}
	}

	void drawPointLight(PointLightComponent& pointLight)
	{
		static bool opened = false;
		if (_beginComponent("Point Light", IconPool::get("point_light"), opened, &pointLight.enabled))
		{
			IMComponents::input("Intensity", pointLight.intensity);
			IMComponents::colorPicker("Color", pointLight.color);
			IMComponents::input("Range", pointLight.range);
			IMComponents::input("Falloff", pointLight.falloff);
		}
	}

	void drawSpotlight(SpotlightComponent& spotlight)
	{
		static bool opened = false;
		if (_beginComponent("Spotlight", IconPool::get("spotlight"), opened, &spotlight.enabled))
		{
			IMComponents::input("Intensity", spotlight.intensity);
			IMComponents::colorPicker("Color", spotlight.color);
			IMComponents::input("Range", spotlight.range);
			IMComponents::input("Falloff", spotlight.falloff);
			IMComponents::input("Inner Angle", spotlight.innerAngle);
			IMComponents::input("Outer Angle", spotlight.outerAngle);
		}
	}

	void drawVelocity(VelocityComponent& velocity)
	{
	}

	void drawBoxCollider(BoxColliderComponent& boxCollider)
	{
	}

	void drawSphereCollider(SphereColliderComponent& sphereCollider)
	{
	}

	void drawRigidbody(RigidbodyComponent& rigidbody)
	{
	}
}
