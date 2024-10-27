project(tactile-renderers-vulkan-lib CXX)

option(TACTILE_USE_VULKAN_SUBSET "Allow partial Vulkan implementations, such as MoltenVK" OFF)

message("TACTILE_USE_VULKAN_SUBSET: ${TACTILE_USE_VULKAN_SUBSET}")

add_library(tactile-vulkan-renderer SHARED)
add_library(tactile::vulkan_renderer ALIAS tactile-vulkan-renderer)

target_sources(tactile-vulkan-renderer
               PRIVATE
               "src/logging.cpp"
               "src/vulkan_allocator.cpp"
               "src/vulkan_buffer.cpp"
               "src/vulkan_command_buffer.cpp"
               "src/vulkan_command_pool.cpp"
               "src/vulkan_descriptor_pool.cpp"
               "src/vulkan_descriptor_set_layout.cpp"
               "src/vulkan_device.cpp"
               "src/vulkan_fence.cpp"
               "src/vulkan_image.cpp"
               "src/vulkan_image_view.cpp"
               "src/vulkan_imgui_context.cpp"
               "src/vulkan_instance.cpp"
               "src/vulkan_physical_device.cpp"
               "src/vulkan_pipeline.cpp"
               "src/vulkan_pipeline_layout.cpp"
               "src/vulkan_renderer.cpp"
               "src/vulkan_renderer_plugin.cpp"
               "src/vulkan_sampler.cpp"
               "src/vulkan_semaphore.cpp"
               "src/vulkan_shader_module.cpp"
               "src/vulkan_surface.cpp"
               "src/vulkan_swapchain.cpp"
               "src/vulkan_texture.cpp"
               "src/vulkan_util.cpp"

               PUBLIC FILE_SET "HEADERS" BASE_DIRS "inc" FILES
               "inc/tactile/vulkan/api.hpp"
               "inc/tactile/vulkan/imgui_shader_code.hpp"
               "inc/tactile/vulkan/logging.hpp"
               "inc/tactile/vulkan/vulkan_allocator.hpp"
               "inc/tactile/vulkan/vulkan_buffer.hpp"
               "inc/tactile/vulkan/vulkan_command_buffer.hpp"
               "inc/tactile/vulkan/vulkan_command_pool.hpp"
               "inc/tactile/vulkan/vulkan_descriptor_pool.hpp"
               "inc/tactile/vulkan/vulkan_descriptor_set_layout.hpp"
               "inc/tactile/vulkan/vulkan_device.hpp"
               "inc/tactile/vulkan/vulkan_fence.hpp"
               "inc/tactile/vulkan/vulkan_image.hpp"
               "inc/tactile/vulkan/vulkan_image_view.hpp"
               "inc/tactile/vulkan/vulkan_imgui_context.hpp"
               "inc/tactile/vulkan/vulkan_instance.hpp"
               "inc/tactile/vulkan/vulkan_physical_device.hpp"
               "inc/tactile/vulkan/vulkan_pipeline.hpp"
               "inc/tactile/vulkan/vulkan_pipeline_layout.hpp"
               "inc/tactile/vulkan/vulkan_renderer.hpp"
               "inc/tactile/vulkan/vulkan_renderer_plugin.hpp"
               "inc/tactile/vulkan/vulkan_sampler.hpp"
               "inc/tactile/vulkan/vulkan_semaphore.hpp"
               "inc/tactile/vulkan/vulkan_shader_module.hpp"
               "inc/tactile/vulkan/vulkan_surface.hpp"
               "inc/tactile/vulkan/vulkan_swapchain.hpp"
               "inc/tactile/vulkan/vulkan_texture.hpp"
               "inc/tactile/vulkan/vulkan_util.hpp"
               )

tactile_prepare_target(tactile-vulkan-renderer)

target_compile_definitions(tactile-vulkan-renderer
                           PRIVATE
                           "TACTILE_BUILDING_VULKAN_RENDERER"
                           )

target_include_directories(tactile-vulkan-renderer
                           PRIVATE
                           ${Stb_INCLUDE_DIR})

target_link_libraries(tactile-vulkan-renderer
                      PUBLIC
                      tactile::base
                      tactile::log
                      Vulkan::Vulkan
                      GPUOpen::VulkanMemoryAllocator

                      PRIVATE
                      imgui::imgui
                      magic_enum::magic_enum
                      SDL2::SDL2
                      )

if (TACTILE_USE_VULKAN_SUBSET)
  target_compile_definitions(tactile-vulkan-renderer
                             PRIVATE
                             "TACTILE_USE_VULKAN_SUBSET"
                             )
endif ()
