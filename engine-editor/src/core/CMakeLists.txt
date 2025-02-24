project(engine-core)

set(SOURCE_FILES
	backend/api.h
	context/application_context.h
	diagnostics/diagnostics.h
	diagnostics/profiler.h
	ecs/components.h
	ecs/ecs.h
	ecs/ecs_collection.h
	ecs/entity_container.hpp
	ecs/reflection.h
	engine.h
	input/cursor.h
	input/input.h
	physics/core/physics_bridge.h
	physics/core/physics_context.h
	physics/physics.h
	physics/rigidbody/rigidbody.h
	physics/rigidbody/rigidbody_enums.h
	physics/utils/px_translator.h
	rendering/culling/bounding_volume.h
	rendering/gizmos/gizmos.h
	rendering/gizmos/gizmo_color.h
	rendering/gizmos/imgizmo.h
	rendering/icons/icon_pool.h
	rendering/material/imaterial.h
	rendering/material/lit/lit_material.h
	rendering/material/unlit/unlit_material.h
	rendering/model/mesh.h
	rendering/model/model.h
	rendering/passes/forward_pass.h
	rendering/passes/preprocessor_pass.h
	rendering/passes/pre_pass.h
	rendering/passes/ssao_pass.h
	rendering/postprocessing/bloom_pass.h
	rendering/postprocessing/motion_blur_pass.h
	rendering/postprocessing/post_processing.h
	rendering/postprocessing/post_processing_pipeline.h
	rendering/primitives/global_quad.h
	rendering/sceneview/scene_view_forward_pass.h
	rendering/shader/shader.h
	rendering/shader/shader_pool.h
	rendering/shadows/shadow_disk.h
	rendering/shadows/shadow_map.h
	rendering/skybox/cubemap.h
	rendering/skybox/skybox.h
	rendering/texture/texture.h
	rendering/transformation/transformation.h
	rendering/velocitybuffer/velocity_buffer.h
	resource/resource.h
	resource/resource_loader.h
	time/time.h
	transform/transform.h
	utils/console.h
	utils/iohandler.h
	utils/string_helper.h
	viewport/viewport.h
	context/application_context.cpp
	diagnostics/diagnostics.cpp
	diagnostics/profiler.cpp
	ecs/ecs.cpp
	ecs/reflection.cpp
	input/cursor.cpp
	input/input.cpp
	misc/stb_image.cpp
	physics/core/physics_bridge.cpp
	physics/core/physics_context.cpp
	physics/rigidbody/rigidbody.cpp
	physics/utils/px_translator.cpp
	rendering/culling/bounding_volume.cpp
	rendering/gizmos/imgizmo.cpp
	rendering/icons/icon_pool.cpp
	rendering/material/lit/lit_material.cpp
	rendering/material/unlit/unlit_material.cpp
	rendering/model/mesh.cpp
	rendering/model/model.cpp
	rendering/passes/forward_pass.cpp
	rendering/passes/preprocessor_pass.cpp
	rendering/passes/pre_pass.cpp
	rendering/passes/ssao_pass.cpp
	rendering/postprocessing/bloom_pass.cpp
	rendering/postprocessing/motion_blur_pass.cpp
	rendering/postprocessing/post_processing_pipeline.cpp
	rendering/primitives/global_quad.cpp
	rendering/sceneview/scene_view_forward_pass.cpp
	rendering/shader/shader.cpp
	rendering/shader/shader_pool.cpp
	rendering/shadows/shadow_disk.cpp
	rendering/shadows/shadow_map.cpp
	rendering/skybox/cubemap.cpp
	rendering/skybox/skybox.cpp
	rendering/texture/texture.cpp
	rendering/transformation/transformation.cpp
	rendering/velocitybuffer/velocity_buffer.cpp
	resource/resource_loader.cpp
	time/time.cpp
	transform/transform.cpp
	utils/console.cpp
	utils/iohandler.cpp
	utils/string_helper.cpp
	viewport/viewport.cpp
)

add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
add_library(nuro::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
	PUBLIC 
		${CMAKE_CURRENT_LIST_DIR}
		${Stb_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} 
	PUBLIC
		unofficial::omniverse-physx-sdk::sdk
		glad::glad 
		glm::glm 
	PRIVATE 
		assimp::assimp
		EnTT::EnTT	
		glfw 
)

if (MSVC)
   target_compile_definitions(${PROJECT_NAME}
    PRIVATE 
      DECL_SPEC_DATA=__declspec\(dllexport\) # build setting for the library: export
      FOO_VAR=0
    INTERFACE
      DECL_SPEC_DATA=__declspec\(dllimport\) # for the consumer: import
      FOO_VAR=1
   )
endif()

# Optional: import the defined target to copy over the GPU acceleration libraries (3rd party provided by NVIDIA)
if(TARGET unofficial::omniverse-physx-sdk::gpu-library)
    if(UNIX)
        # Add rpath setting to find .so libraries on unix based systems
        set_target_properties(${PROJECT_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "$ORIGIN"
        )
    endif()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-library>
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-device-library>
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()
else()
    message(WARNING "GPU acceleration library target not defined - GPU acceleration will NOT be available!")
endif()

