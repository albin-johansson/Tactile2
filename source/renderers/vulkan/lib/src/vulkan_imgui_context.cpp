// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_imgui_context.hpp"

#include <expected>  // expected
#include <utility>   // move

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include "tactile/base/render/window.hpp"
#include "tactile/base/util/scope_exit.hpp"
#include "tactile/vulkan/imgui_shader_code.hpp"
#include "tactile/vulkan/logging.hpp"
#include "tactile/vulkan/vulkan_shader_module.hpp"
#include "tactile/vulkan/vulkan_util.hpp"

namespace tactile::vk {
namespace {

auto _create_imgui_descriptor_set_layout(VkDevice device)
    -> std::expected<VulkanDescriptorSetLayout, VkResult>
{
  VulkanDescriptorSetLayout layout {};
  layout.device = device;

  constexpr VkDescriptorSetLayoutBinding binding {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    .pImmutableSamplers = nullptr,
  };

  const VkDescriptorSetLayoutCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .bindingCount = 1,
    .pBindings = &binding,
  };

  const auto result =
      vkCreateDescriptorSetLayout(device, &create_info, nullptr, &layout.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan ImGui descriptor set layout: {}",
                         to_string(result));
    return std::unexpected {result};
  }

  return layout;
}

auto _create_imgui_pipeline_layout(VkDevice device,
                                   VkDescriptorSetLayout descriptor_set_layout)
    -> std::expected<VulkanPipelineLayout, VkResult>
{
  constexpr VkPushConstantRange push_constant_range {
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = 4 * sizeof(float),
  };

  const VkPipelineLayoutCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &descriptor_set_layout,
    .pushConstantRangeCount = 1,
    .pPushConstantRanges = &push_constant_range,
  };

  VulkanPipelineLayout layout {};
  layout.device = device;

  const auto result = vkCreatePipelineLayout(device, &create_info, nullptr, &layout.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan ImGui pipeline layout: {}",
                         to_string(result));
    return std::unexpected {result};
  }

  return layout;
}

auto _create_imgui_graphics_pipeline(VkDevice device,
                                     VkPipelineLayout pipeline_layout,
                                     const VkPipelineRenderingCreateInfoKHR& rendering_info)
    -> std::expected<VulkanPipeline, VkResult>
{
  VulkanPipeline pipeline {};
  pipeline.device = device;

  const auto vertex_shader = create_vulkan_shader_module(device, kVulkanImGuiVertexShaderCode);
  if (!vertex_shader) {
    return std::unexpected {vertex_shader.error()};
  }

  const auto fragment_shader = create_vulkan_shader_module(device, kVulkanImGuiFragShaderCode);
  if (!fragment_shader) {
    return std::unexpected {fragment_shader.error()};
  }

  const VkPipelineShaderStageCreateInfo shader_stages[] = {
    VkPipelineShaderStageCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertex_shader->handle,
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
    VkPipelineShaderStageCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_shader->handle,
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
  };

  constexpr VkVertexInputBindingDescription vertex_input_binding {
    .binding = 0,
    .stride = sizeof(ImDrawVert),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  constexpr VkVertexInputAttributeDescription vertex_input_attributes[] = {
    VkVertexInputAttributeDescription {
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(ImDrawVert, pos),
    },
    VkVertexInputAttributeDescription {
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = offsetof(ImDrawVert, uv),
    },
    VkVertexInputAttributeDescription {
      .location = 2,
      .binding = 0,
      .format = VK_FORMAT_R8G8B8A8_UNORM,
      .offset = offsetof(ImDrawVert, col),
    },
  };

  const VkPipelineVertexInputStateCreateInfo vertex_input_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &vertex_input_binding,
    .vertexAttributeDescriptionCount = 3,
    .pVertexAttributeDescriptions = vertex_input_attributes,
  };

  constexpr VkPipelineInputAssemblyStateCreateInfo input_assembly {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  constexpr VkPipelineViewportStateCreateInfo viewport_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = nullptr,
    .scissorCount = 1,
    .pScissors = nullptr,
  };

  constexpr VkPipelineRasterizationStateCreateInfo rasterization_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_NONE,  // Dear ImGui doesn't work well with back-face culling
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp = VK_FALSE,
    .depthBiasSlopeFactor = 0.0f,
    .lineWidth = 1.0f,
  };

  constexpr VkPipelineMultisampleStateCreateInfo multisample_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0.0f,
    .pSampleMask = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE,
  };

  constexpr VkPipelineColorBlendAttachmentState color_blend_attachment {
    .blendEnable = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  constexpr VkPipelineDepthStencilStateCreateInfo depth_stencil_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
    .front = VkStencilOpState {},
    .back = VkStencilOpState {},
    .minDepthBounds = 0.0f,
    .maxDepthBounds = 0.0f,
  };

  const VkPipelineColorBlendStateCreateInfo color_blend_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_NO_OP,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment,
    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  constexpr VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  const VkPipelineDynamicStateCreateInfo dynamic_state {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .dynamicStateCount = 2,
    .pDynamicStates = dynamic_states,
  };

  const VkGraphicsPipelineCreateInfo create_info {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &rendering_info,
    .flags = 0,
    .stageCount = 2,
    .pStages = shader_stages,
    .pVertexInputState = &vertex_input_state,
    .pInputAssemblyState = &input_assembly,
    .pTessellationState = nullptr,
    .pViewportState = &viewport_state,
    .pRasterizationState = &rasterization_state,
    .pMultisampleState = &multisample_state,
    .pDepthStencilState = &depth_stencil_state,
    .pColorBlendState = &color_blend_state,
    .pDynamicState = &dynamic_state,
    .layout = pipeline_layout,
    .renderPass = nullptr,
    .subpass = 0,
    .basePipelineHandle = nullptr,
    .basePipelineIndex = 0,
  };

  const auto result =
      vkCreateGraphicsPipelines(device, nullptr, 1, &create_info, nullptr, &pipeline.handle);

  if (result != VK_SUCCESS) {
    TACTILE_VULKAN_ERROR("Could not create Vulkan ImGui pipeline: {}", to_string(result));
    return std::unexpected {result};
  }

  return pipeline;
}

}  // namespace

auto VulkanImGuiContext::init(IWindow& window, ImGui_ImplVulkan_InitInfo& vulkan_info)
    -> std::optional<VulkanImGuiContext>
{
  VulkanImGuiContext context {};

  context.m_ctx = ImGui::CreateContext();
  if (!context.m_ctx) {
    TACTILE_VULKAN_ERROR("Could not create ImGui context");
    return std::nullopt;
  }

  context.m_context_deleter = ScopeExit {[ctx = context.m_ctx] {
    TACTILE_VULKAN_TRACE("ImGui::DestroyContext");
    ImGui::DestroyContext(ctx);
  }};

  if (!ImGui_ImplSDL2_InitForVulkan(window.get_handle())) {
    TACTILE_VULKAN_ERROR("Could not initialize SDL2 ImGui backend implementation");
    return std::nullopt;
  }

  context.m_backend_impl_deleter = ScopeExit {[] {
    TACTILE_VULKAN_TRACE("ImGui_ImplSDL2_Shutdown");
    ImGui_ImplSDL2_Shutdown();
  }};

  if (!ImGui_ImplVulkan_Init(&vulkan_info)) {
    TACTILE_VULKAN_ERROR("Could not initialize Vulkan ImGui renderer implementation");
    return std::nullopt;
  }

  context.m_renderer_impl_deleter = ScopeExit {[] {
    TACTILE_VULKAN_TRACE("ImGui_ImplVulkan_Shutdown");
    ImGui_ImplVulkan_Shutdown();
  }};

  if (auto descriptor_set_layout = _create_imgui_descriptor_set_layout(vulkan_info.Device)) {
    context.m_descriptor_set_layout = std::move(*descriptor_set_layout);
  }
  else {
    return std::nullopt;
  }

  if (auto pipeline_layout =
          _create_imgui_pipeline_layout(vulkan_info.Device,
                                        context.m_descriptor_set_layout.handle)) {
    context.m_pipeline_layout = std::move(*pipeline_layout);
  }
  else {
    return std::nullopt;
  }

  if (auto pipeline =
          _create_imgui_graphics_pipeline(vulkan_info.Device,
                                          context.m_pipeline_layout.handle,
                                          vulkan_info.PipelineRenderingCreateInfo)) {
    context.m_pipeline = std::move(*pipeline);
  }
  else {
    return std::nullopt;
  }

  return context;
}

auto VulkanImGuiContext::get() const -> ImGuiContext*
{
  return m_ctx;
}

auto VulkanImGuiContext::get_pipeline() const -> VkPipeline
{
  return m_pipeline.handle;
}

}  // namespace tactile::vk
