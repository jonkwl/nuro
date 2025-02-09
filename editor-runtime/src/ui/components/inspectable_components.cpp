#include "inspectable_components.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include <imgui.h>
#include <cstdint>
#include <unordered_map>

#include "../core/rendering/icons/icon_pool.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/collection/IconsFontAwesome6.h"

namespace InspectableComponents {

	// A component id is the numeric representation of its hashed identifier
	using ComponentID = uint32_t;

	// Stores if components are opened or closed
	std::unordered_map<ComponentID, bool> g_Opened;

	// Returns if current component is opened
	bool* _isOpened(ComponentID component) {
		return &g_Opened[component];
	}

	bool _beginComponent(std::string identifier, uint32_t icon, bool* enabledPtr, bool alwaysOpened = false)
	{
		//
		// EVALUATE
		//

		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		ImVec2 contentAvail = ImGui::GetContentRegionAvail();
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		float titleHeight = EditorSizing::h4_FontSize;
		ImVec2 titlePadding = ImVec2(10.0f, 10.0f);

		float height = titleHeight + titlePadding.y * 2;
		float yMargin = 2.0f;
		ImVec2 size = ImVec2(contentAvail.x, height);
		
		ImVec2 p0 = ImVec2(cursorPos.x, cursorPos.y + yMargin);
		ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);
		const bool hovered = ImGui::IsMouseHoveringRect(p0, p1);

		ImVec2 cursor = p0 + titlePadding;

		bool alwaysEnabled = !enabledPtr;

		ComponentID componentId = entt::hashed_string::value(identifier.c_str());
		bool* openedPtr = _isOpened(componentId);

		//
		// CLICK COLLAPSE
		//

		if (!alwaysOpened) {
			if (hovered && ImGui::IsMouseClicked(2)) *openedPtr = !(*openedPtr);
		}

		//
		// DRAW BACKGROUND
		//

		drawList.AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 255), 10.0f);

		//
		// DRAW EXPANSION CARET
		//

		if (alwaysOpened) {
			drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, cursor, EditorColor::text, ICON_FA_CARET_DOWN);
		}
		else {
			IMComponents::caret(*openedPtr, drawList, cursor, ImVec2(-1.0f, 2.0f), IM_COL32(0, 0, 0, 0), EditorColor::element);
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

		if (!alwaysEnabled) {

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

		drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, cursor, EditorColor::text, identifier.c_str());

		//
		// ADVANCE CURSOR AND DRAW CUSTOM CONTENT
		//

		bool currentlyOpened = alwaysOpened ? true : *openedPtr;

		if (currentlyOpened) {
			ImGui::Dummy(ImVec2(0.0f, size.y + yMargin + 12.0f));
		}
		else {
			ImGui::Dummy(ImVec2(size.x, size.y + yMargin));
		}

		return currentlyOpened;
	}

	void _endComponent() {
		ImGui::Dummy(ImVec2(0.0f, 15.0f));
	}

	void _headline(std::string label) {
		IMComponents::label(label, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 1.0f));
	}

	void _spacing() {
		ImGui::Dummy(ImVec2(0.0f, 6.0f));
	}

	void drawTransform(TransformComponent& transform)
	{
		if (_beginComponent("Transform", IconPool::get("transform"), nullptr, true))
		{
			_headline("General");
			glm::vec3 rotationTmp = glm::degrees(glm::eulerAngles(transform.rotation));
			IMComponents::input("Position", transform.position);
			IMComponents::input("Rotation", rotationTmp);
			IMComponents::input("Scale", transform.scale);

			_endComponent();
		}
	}

	void drawMeshRenderer(MeshRendererComponent& meshRenderer)
	{
		if (_beginComponent("Mesh Renderer", IconPool::get("mesh_renderer"), &meshRenderer.enabled))
		{
			_endComponent();
		}
	}

	void drawCamera(CameraComponent& camera)
	{
		if (_beginComponent("Camera", IconPool::get("camera"), &camera.enabled))
		{
			_headline("General");
			IMComponents::input("FOV", camera.fov);
			IMComponents::input("Near", camera.near);
			IMComponents::input("Far", camera.far);

			_endComponent();
		}
	}

	void drawDirectionalLight(DirectionalLightComponent& directionalLight)
	{
		if (_beginComponent("Directional Light", IconPool::get("directional_light"), &directionalLight.enabled))
		{
			_headline("General");
			IMComponents::input("Intensity", directionalLight.intensity);
			IMComponents::colorPicker("Color", directionalLight.color);

			_endComponent();
		}
	}

	void drawPointLight(PointLightComponent& pointLight)
	{
		if (_beginComponent("Point Light", IconPool::get("point_light"), &pointLight.enabled))
		{
			_headline("General");
			IMComponents::input("Intensity", pointLight.intensity);
			IMComponents::colorPicker("Color", pointLight.color);
			IMComponents::input("Range", pointLight.range);
			IMComponents::input("Falloff", pointLight.falloff);

			_endComponent();
		}
	}

	void drawSpotlight(SpotlightComponent& spotlight)
	{
		if (_beginComponent("Spotlight", IconPool::get("spotlight"), &spotlight.enabled))
		{
			_headline("General");
			IMComponents::input("Intensity", spotlight.intensity);
			IMComponents::colorPicker("Color", spotlight.color);
			IMComponents::input("Range", spotlight.range);
			IMComponents::input("Falloff", spotlight.falloff);
			IMComponents::input("Inner Angle", spotlight.innerAngle);
			IMComponents::input("Outer Angle", spotlight.outerAngle);

			_endComponent();
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
