//
// Created by kyros on 5/29/23.
//

/*
* Vulkan buffer class
*
* Encapsulates a Vulkan buffer
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef XEXAMPLE_RENDER_BUFFER_H
#define XEXAMPLE_RENDER_BUFFER_H

#include "vulkan/vulkan.h"
#include "vulkan_context.h"
#include <memory>

namespace VulkanAPI
{
    class Buffer
    {
    public:
        std::shared_ptr<VulkanContext> p_context;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDescriptorBufferInfo descriptor;
        VkDeviceSize size = 0;
        VkDeviceSize alignment = 0;
        void* mapped = nullptr;
        /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
        VkBufferUsageFlags usageFlags;
        /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
        VkMemoryPropertyFlags memoryPropertyFlags;
        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();
        VkResult bind(VkDeviceSize offset = 0);
        void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void copyTo(void* data, VkDeviceSize size);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void destroy();
        void initialize(std::shared_ptr<VulkanContext> context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    };
}

#endif //XEXAMPLE_RENDER_BUFFER_H
