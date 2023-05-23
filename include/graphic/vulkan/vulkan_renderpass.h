//
// Created by kyrosz7u on 4/7/23.
//

#ifndef VULKANRENDER_VULKAN_RENDERPASS_H
#define VULKANRENDER_VULKAN_RENDERPASS_H

#include "vulkan_subpass.h"

class RenderCommandInfo;

namespace VulkanAPI
{
    struct ImageAttachment
    {
        VkImage        image;
        VkDeviceMemory mem;
        VkImageView    view;
        VkFormat       format;
        VkImageLayout  layout; // 使用图像之前，需要将其转换为适当的布局
    };

    struct Framebuffer
    {
        int           width;
        int           height;
        VkFramebuffer framebuffer;

        std::vector<ImageAttachment> attachments;
    };

    class RenderPassInitInfo
    {
    public:
        RenderPassInitInfo* renderpass_init_info
        std::vector<ImageAttachment> renderTargets;
        std::vector<VkClearValue> clearValues;
//        std::vector<VkSubpassDependency> subpassDependencies;
//        std::vector<VkSubpassDescription> subpassDescriptions;
    };

    class RenderPassBase
    {
    public:
        RenderPassBase()
        {
            assert(m_p_vulkan_context!= nullptr);
        }
        
        static void setVulkanContext(std::shared_ptr<VulkanContext> vulkanContext)
        {
            m_p_vulkan_context = vulkanContext;
        }

        virtual void initialize(RenderPassInitInfo* renderpass_init_info) = 0;
        virtual void setupSubpass() = 0;
        virtual void draw(int render_target_index, VkCommandBuffer command_buffer) = 0;
    protected:
        static std::shared_ptr<VulkanContext> m_p_vulkan_context;
        RenderCommandInfo* m_p_render_command_info;
        std::vector<std::shared_ptr<VulkanSubPassBase>> m_subpass_list;
    };
}

#endif //VULKANRENDER_VULKAN_RENDERPASS_H
