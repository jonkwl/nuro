#include "inspectable_components.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include <imgui.h>
#include <cstdint>
#include <unordered_map>

#include <transform/transform.h>
#include <rendering/icons/icon_pool.h>

#include "../ui/editor_ui.h"
#include "../ui/components/im_components.h"
#include "../ui/collection/IconsFontAwesome6.h"

namespace InspectableComponents {

	// A component id is the numeric representation of its hashed identifier
	using ComponentID = uint32_t;

	// Stores if components are opened or closed
	std::unordered_map<ComponentID, bool> g_Opened;

	// Returns if current component is opened
	bool* _isOpened(ComponentID component) {
		return &g_Opened[component];
	}

	// Draws a component element and returns true if its expanded. Pointer parameters can be set to nullptr to disable the corresponding feature (e.g., removedPtr set as nullptr means component cant be removed)
	bool _beginComponent(std::string identifier, uint32_t icon, bool* enabledPtr = nullptr, bool* removedPtr = nullptr, bool alwaysOpened = false)
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
		// DRAW REMOVE BUTTON
		//

		if (removedPtr) {
			float size = 20.0f;
			cursor.x = p1.x - size - titlePadding.x;
			if (IMComponents::iconButton(ICON_FA_XMARK, drawList, cursor, ImVec2(-1.0f, 2.0f), IM_COL32(0, 0, 0, 0), IM_COL32(255, 65, 65, 90))) {
				*removedPtr = true;
			}
		}

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
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
	}

	void _headline(std::string label) {
		IMComponents::label(label, EditorUI::getFonts().h4_bold);
		ImGui::Dummy(ImVec2(0.0f, 1.0f));
	}

	void _spacingS() {
		ImGui::Dummy(ImVec2(0.0f, 3.0));
	}

	void _spacingM() {
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
	}

	void drawTransform(Entity entity, TransformComponent& transform)
	{
		if (_beginComponent("Transform", IconPool::get("transform"), nullptr, nullptr, true))
		{
			_headline("Properties");
			IMComponents::input("Position", transform.position);
			IMComponents::input("Rotation", transform.eulerAngles);
			IMComponents::input("Scale", transform.scale);
			if (Transform::hasParent(transform)) IMComponents::label("Parent: " + Transform::fetchParent(transform).name);
			IMComponents::label("ID: " + std::to_string(transform.id));
			IMComponents::label("Depth: " + std::to_string(transform.depth));

			if (Transform::hasParent(transform)) {
				glm::vec3 worldPos = Transform::getPosition(transform, Space::WORLD);
				glm::vec3 worldScale = Transform::getScale(transform, Space::WORLD);
				IMComponents::label("World Position X: " + std::to_string(worldPos.x) + " Y: " + std::to_string(worldPos.y) + " Z: " + std::to_string(worldPos.z));
				IMComponents::label("World Scale X: " + std::to_string(worldScale.x) + " Y: " + std::to_string(worldScale.y) + " Z: " + std::to_string(worldScale.z));
			}

			// tmp
			Transform::setPosition(transform, transform.position);
			Transform::setEulerAngles(transform, transform.eulerAngles);
			Transform::setScale(transform, transform.scale);

			_endComponent();
		}
	}

	void drawMeshRenderer(Entity entity, MeshRendererComponent& meshRenderer)
	{
		bool removed = false;

		if (_beginComponent("Mesh Renderer", IconPool::get("mesh_renderer"), &meshRenderer.enabled, &removed))
		{
			_headline("General");

			if (meshRenderer.mesh) {
				IMComponents::label("Mesh ID: " + std::to_string(meshRenderer.mesh->getVAO()));
			}

			if (meshRenderer.material) {
				IMComponents::label("Material ID: " + std::to_string(meshRenderer.material->getId()));
			}

			_endComponent();
		}

		if (removed) ECS::main().remove<MeshRendererComponent>(entity);
	}

	void drawCamera(Entity entity, CameraComponent& camera)
	{
		bool removed = false;

		if (_beginComponent("Camera", IconPool::get("camera"), &camera.enabled, &removed))
		{
			_headline("General");
			IMComponents::input("FOV", camera.fov);
			IMComponents::input("Near", camera.near);
			IMComponents::input("Far", camera.far);

			_endComponent();
		}

		if (removed) ECS::main().remove<CameraComponent>(entity);
	}

	void drawDirectionalLight(Entity entity, DirectionalLightComponent& directionalLight)
	{
		bool removed = false;

		if (_beginComponent("Directional Light", IconPool::get("directional_light"), &directionalLight.enabled, &removed))
		{
			_headline("Properties");
			IMComponents::input("Intensity", directionalLight.intensity);
			IMComponents::colorPicker("Color", directionalLight.color);

			_spacingM();
			_headline("Shadows");
			bool tmp = false;
			IMComponents::input("Cast Shadows", tmp);
			IMComponents::input("Soft Shadows", tmp);
			IMComponents::label(ICON_FA_TRIANGLE_EXCLAMATION " In-editor shadows aren't dynamic yet and can't be changed currently", EditorUI::getFonts().p, IM_COL32(255, 255, 0, 135));

			_endComponent();
		}

		if (removed) ECS::main().remove<DirectionalLightComponent>(entity);
	}

	void drawPointLight(Entity entity, PointLightComponent& pointLight)
	{
		bool removed = false;

		if (_beginComponent("Point Light", IconPool::get("point_light"), &pointLight.enabled, &removed))
		{
			_headline("Properties");
			IMComponents::input("Intensity", pointLight.intensity);
			IMComponents::colorPicker("Color", pointLight.color);
			IMComponents::input("Range", pointLight.range);
			IMComponents::input("Falloff", pointLight.falloff);

			_spacingM();
			_headline("Shadows");
			bool tmp = false;
			IMComponents::input("Cast Shadows", tmp);
			IMComponents::input("Soft Shadows", tmp);
			IMComponents::label(ICON_FA_TRIANGLE_EXCLAMATION " In-editor shadows aren't dynamic yet and can't be changed currently", EditorUI::getFonts().p, IM_COL32(255, 255, 0, 135));

			_endComponent();
		}

		if (removed) ECS::main().remove<PointLightComponent>(entity);
	}
	
	void drawSpotlight(Entity entity, SpotlightComponent& spotlight)
	{
		bool removed = false;

		if (_beginComponent("Spotlight", IconPool::get("spotlight"), &spotlight.enabled, &removed))
		{
			_headline("Properties");
			IMComponents::input("Intensity", spotlight.intensity);
			IMComponents::colorPicker("Color", spotlight.color);
			IMComponents::input("Range", spotlight.range);
			IMComponents::input("Falloff", spotlight.falloff);
			IMComponents::input("Inner Angle", spotlight.innerAngle);
			IMComponents::input("Outer Angle", spotlight.outerAngle);

			_spacingM();
			_headline("Shadows");
			bool tmp = true;
			IMComponents::input("Cast Shadows", tmp);
			IMComponents::input("Soft Shadows", tmp);
			IMComponents::label(ICON_FA_TRIANGLE_EXCLAMATION " In-editor shadows aren't dynamic yet and can't be changed currently", EditorUI::getFonts().p, IM_COL32(255, 255, 0, 135));

			_endComponent();
		}

		if (removed) ECS::main().remove<SpotlightComponent>(entity);
	}

	void drawVelocity(Entity entity, VelocityComponent& velocity)
	{
		bool removed = false;

		if (_beginComponent("Velocity Blur", IconPool::get("velocity_blur"), &velocity.enabled, &removed))
		{
			_headline("General");
			IMComponents::input("Intensity", velocity.intensity);

			_endComponent();
		}

		if (removed) ECS::main().remove<VelocityComponent>(entity);
	}

	void drawBoxCollider(Entity entity, BoxColliderComponent& boxCollider)
	{
		bool removed = false;

		if (_beginComponent("Box Collider", IconPool::get("box_collider"), nullptr, &removed))
		{
			_headline("Transform");
			IMComponents::buttonBig("Reset Dimensions");
			_spacingS();
			IMComponents::input("Center", boxCollider.center);
			IMComponents::input("Size", boxCollider.size);
			
			_spacingM();
			_headline("Material");
			IMComponents::label("Material: Default Physics Material");

			_endComponent();
		}

		if (removed) ECS::main().remove<BoxColliderComponent>(entity);
	}

	void drawSphereCollider(Entity entity, SphereColliderComponent& sphereCollider)
	{
		bool removed = false;

		if (_beginComponent("Sphere Collider", IconPool::get("sphere_collider"), nullptr, &removed))
		{
			_headline("Transform");
			IMComponents::buttonBig("Reset Dimensions");
			_spacingS();
			IMComponents::input("Center", sphereCollider.center);
			IMComponents::input("Radius", sphereCollider.radius);

			_spacingM();
			_headline("Material");
			IMComponents::label("Material: Default Physics Material");

			_endComponent();
		}

		if (removed) ECS::main().remove<SphereColliderComponent>(entity);
	}

	void drawRigidbody(Entity entity, RigidbodyComponent& rigidbody)
	{
		bool removed = false;

		if (_beginComponent("Rigidbody", IconPool::get("rigidbody"), nullptr, &removed))
		{
			_headline("Properties");
			IMComponents::input("Mass", rigidbody.mass);
			IMComponents::input("Resistance", rigidbody.resistance);
			IMComponents::input("Angular Resistance", rigidbody.angularResistance);
			IMComponents::input("Use Gravity", rigidbody.gravity);
			IMComponents::input("Is Kinematic", rigidbody.kinematic);

			_endComponent();
		}

		if (removed) ECS::main().remove<RigidbodyComponent>(entity);
	}

	void drawColor(PostProcessing::Color& color)
	{
		if (_beginComponent("Color", IconPool::get("color"), nullptr, nullptr))
		{
			_spacingS();
			_headline("Color Settings");
			_spacingM();

			IMComponents::input("Exposure", color.exposure);
			IMComponents::input("Contrast", color.contrast);
			IMComponents::input("Gamma", color.gamma);

			_endComponent();
		}
	}

	void drawMotionBlur(PostProcessing::MotionBlur& motionBlur)
	{
		if (_beginComponent("Motion Blur", IconPool::get("motion_blur"), &motionBlur.enabled, nullptr))
		{
			_spacingS();
			_headline("Motion Blur Settings");
			_spacingM();

			_headline("Camera Space");
			IMComponents::input("Enable", motionBlur.cameraEnabled);
			IMComponents::input("Intensity", motionBlur.cameraIntensity);
			IMComponents::input("Samples", motionBlur.cameraSamples);
			_spacingM();

			_headline("Object Space");
			IMComponents::input("Enable", motionBlur.objectEnabled);
			IMComponents::input("Samples", motionBlur.objectSamples);

			if (motionBlur.objectEnabled) {
				std::string objectInfo = std::string(ICON_FA_PENCIL) + " Objects to be blurred require a 'Velocity' component.";
				IMComponents::label(objectInfo);
			}

			_endComponent();
		}
	}

	void drawBloom(PostProcessing::Bloom& bloom)
	{
		if (_beginComponent("Bloom", IconPool::get("bloom"), &bloom.enabled, nullptr))
		{
			_spacingS();
			_headline("Bloom Settings");
			_spacingM();

			_headline("General");
			IMComponents::input("Intensity", bloom.intensity);
			IMComponents::colorPicker("Color", bloom.color);
			IMComponents::input("Filter Radius", bloom.filterRadius);
			_spacingM();

			_headline("Threshold");
			static bool useThreshold = bloom.threshold;
			IMComponents::input("Use Threshold", useThreshold);

			if (useThreshold) {
				IMComponents::input("Threshold", bloom.threshold);
				IMComponents::input("Soft Threshold", bloom.softThreshold);
			}
			else {
				bloom.threshold = 0.0f;
				bloom.softThreshold = 0.0f;
			}
			_spacingM();

			_headline("Lens Dirt");
			static bool enableLensDirt = bloom.lensDirtEnabled;
			IMComponents::input("Enable Lens Dirt", enableLensDirt);

			if (enableLensDirt) {
				bloom.lensDirtEnabled = true;
				IMComponents::label("Texture: None");
				IMComponents::input("Intensity", bloom.lensDirtIntensity);
			}
			else {
				bloom.lensDirtEnabled = false;
			}

			_endComponent();
		}
	}

	void drawChromaticAberration(PostProcessing::ChromaticAberration& chromaticAberration)
	{
		if (_beginComponent("Chromatic Aberration", IconPool::get("chromatic_aberration"), &chromaticAberration.enabled, nullptr))
		{
			_spacingS();
			_headline("Chromatic Aberration Settings");
			_spacingM();

			IMComponents::input("Intensity", chromaticAberration.intensity);
			IMComponents::input("Iterations", chromaticAberration.iterations);

			_endComponent();
		}
	}

	void drawVignette(PostProcessing::Vignette& vignette)
	{
		if (_beginComponent("Vignette", IconPool::get("vignette"), &vignette.enabled, nullptr))
		{
			_spacingS();
			_headline("Vignette Settings");
			_spacingM();

			// IMComponents::input("Intensity", vignette.intensity); // Not available yet
			IMComponents::colorPicker("Color", vignette.color);
			IMComponents::input("Radius", vignette.radius);
			IMComponents::input("Softness", vignette.softness);
			IMComponents::input("Roundness", vignette.roundness);

			_endComponent();
		}
	}

	void drawAmbientOcclusion(PostProcessing::AmbientOcclusion& ambientOcclusion)
	{
		if (_beginComponent("Ambient Occlusion", IconPool::get("ambient_occlusion"), &ambientOcclusion.enabled, nullptr))
		{
			_spacingS();
			_headline("Ambient Occlusion Settings");
			_spacingM();

			IMComponents::input("Radius", ambientOcclusion.radius);
			IMComponents::input("Power", ambientOcclusion.power);
			IMComponents::input("Bias", ambientOcclusion.bias);
			IMComponents::input("Samples", ambientOcclusion.samples);

			_endComponent();
		}
	}
}
