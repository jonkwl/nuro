project(nuro-editor)

set(SOURCE_FILES 
	testing/game_logic.h
	pipelines/game_view_pipeline.h
	pipelines/preview_pipeline.h
	pipelines/scene_view_pipeline.h
	project/project.h
	runtime/runtime.h
	ui/collection/IconsFontAwesome6.h
	ui/components/im_components.h
	ui/components/inspectable_components.h
	ui/components/toggle_bar.h
	ui/context_menu/context_menu.h
	ui/dynamic_drawing/delta.h
	ui/dynamic_drawing/dynamic_content.h
	ui/dynamic_drawing/dynamic_drawing.h
	ui/dynamic_drawing/dynamic_text.h
	ui/dynamic_drawing/enums.h
	ui/dynamic_drawing/utils.h
	ui/editor_ui.h
	ui/footer/footer.h
	ui/inspectables/entity_inspectable.h
	ui/inspectables/inspectable.h
	ui/inspectables/welcome_inspectable.h
	ui/title_bar/title_bar.h
	ui/utils/ui_utils.h
	ui/windows/asset_browser_window.h
	ui/windows/console_window.h
	ui/windows/diagnostics_window.h
	ui/windows/editor_window.h
	ui/windows/game_window.h
	ui/windows/insight_panel_window.h
	ui/windows/post_processing_window.h
	ui/windows/registry_window.h
	ui/windows/viewport_window.h
	ui/workspace_bar/workspace_bar.h
	testing/game_logic.cpp
	gizmos/component_gizmos.h
	gizmos/editor_gizmo_color.h
	reflection/component_registry.h
	ui/search/search_popup.h

	main.cpp
	pipelines/game_view_pipeline.cpp
	pipelines/preview_pipeline.cpp
	pipelines/scene_view_pipeline.cpp
	project/project.cpp
	runtime/runtime.cpp
	ui/components/im_components.cpp
	ui/components/inspectable_components.cpp
	ui/components/toggle_bar.cpp
	ui/context_menu/context_menu.cpp
	ui/dynamic_drawing/dynamic_content.cpp
	ui/dynamic_drawing/dynamic_text.cpp
	ui/editor_ui.cpp
	ui/footer/footer.cpp
	ui/inspectables/entity_inspectable.cpp
	ui/inspectables/welcome_inspectable.cpp
	ui/title_bar/title_bar.cpp
	ui/utils/ui_utils.cpp
	ui/windows/asset_browser_window.cpp
	ui/windows/console_window.cpp
	ui/windows/diagnostics_window.cpp
	ui/windows/game_window.cpp
	ui/windows/insight_panel_window.cpp
	ui/windows/post_processing_window.cpp
	ui/windows/registry_window.cpp
	ui/windows/viewport_window.cpp
	ui/workspace_bar/workspace_bar.cpp
	gizmos/component_gizmos.cpp
	reflection/component_registry.cpp
	ui/search/search_popup.cpp
)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/ImGuizmo.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/implot.cpp
    imgui/implot_demo.cpp
    imgui/implot_items.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC 
		${CMAKE_CURRENT_LIST_DIR}/src
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/imgui)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		nuro::core
		glm::glm
		nlohmann_json::nlohmann_json
		# imgui::imgui           # imported from local directory
		# implot::implot         # imported from local directory
		# imguizmo::imguizmo     # imported from local directory
)

add_custom_command(TARGET nuro-editor POST_BUILD
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different  "${CMAKE_SOURCE_DIR}/nuro-editor/layouts/imgui.ini" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/resources/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/nuro-editor/examples/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/examples/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/nuro-core/shaders/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/shaders/"

	COMMAND "${CMAKE_COMMAND}" -E copy_if_different  "${CMAKE_SOURCE_DIR}/nuro-editor/layouts/imgui.ini" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<CONFIG>/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/resources/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<CONFIG>/resources/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/nuro-editor/examples/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<CONFIG>/examples/"
	COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/nuro-core/shaders/" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/$<CONFIG>/shaders/"
)