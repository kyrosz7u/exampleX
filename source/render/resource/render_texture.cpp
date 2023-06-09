//
// Created by kyrosz7u on 2023/6/7.
//

#include "render/resource/render_texture.h"
#include "core/graphic/vulkan/vulkan_utils.h"
#include "core/logger/logger_macros.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <cmath>
#include <stdexcept>

using namespace RenderSystem;
using namespace VulkanAPI;

Texture2D::Texture2D(const std::string &texture_path, const std::string &texture_name, bool gen_mipmap,
                     VkFormat image_format)
{
    name = texture_name;
    path = texture_path;

    int     image_width, image_height, texChannels;
    stbi_uc *pixels = stbi_load(path.c_str(),
                                &image_width,
                                &image_height,
                                &texChannels,
                                STBI_rgb_alpha);
    width      = image_width;
    height     = image_height;
    mip_levels = gen_mipmap ? static_cast<uint32_t>(std::floor(std::log2(std::max(image_width, image_height)))) + 1 : 1;

    VkDeviceSize texture_layer_byte_size;
    switch (image_format)
    {
        case VK_FORMAT_R8G8B8_UNORM:
            texture_layer_byte_size = image_width * image_height * 3;
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            texture_layer_byte_size = image_width * image_height * 3;
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            texture_layer_byte_size = image_width * image_height * 4;
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            texture_layer_byte_size = image_width * image_height * 4;
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            texture_layer_byte_size = image_width * image_height * 4 * 2;
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            texture_layer_byte_size = image_width * image_height * 4 * 3;
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            texture_layer_byte_size = image_width * image_height * 4 * 4;
            break;
        default:
            texture_layer_byte_size = VkDeviceSize(-1);
            throw std::runtime_error("invalid texture_layer_byte_size");
    }

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(g_p_vulkan_context, texture_layer_byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(g_p_vulkan_context->_device, stagingBufferMemory, 0, texture_layer_byte_size, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(texture_layer_byte_size));
    vkUnmapMemory(g_p_vulkan_context->_device, stagingBufferMemory);

    free(pixels);

    VulkanUtil::createImage(g_p_vulkan_context,
                            image_width, image_height,
                            VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory,
                            0, 1, mip_levels);

    VulkanUtil::transitionImageLayout(g_p_vulkan_context,
                                      image,
                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      1, mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);

    VulkanUtil::copyBufferToImage(g_p_vulkan_context,
                                  stagingBuffer,
                                  image,
                                  static_cast<uint32_t>(image_width),
                                  static_cast<uint32_t>(image_height), 1);

    VulkanUtil::transitionImageLayout(g_p_vulkan_context,
                                      image,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      1, mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);

    vkFreeMemory(g_p_vulkan_context->_device, stagingBufferMemory, nullptr);
    vkDestroyBuffer(g_p_vulkan_context->_device, stagingBuffer, nullptr);

    if (gen_mipmap)
    {
        VulkanUtil::genMipmappedImage(g_p_vulkan_context, image, image_width, image_height, mip_levels);
    }
    if (gen_mipmap)
    {
        sampler = VulkanUtil::getOrCreateLinearSampler(g_p_vulkan_context);
    } else
    {
        sampler = VulkanUtil::getOrCreateMipmapSampler(g_p_vulkan_context, mip_levels);
    }
    view = VulkanUtil::createImageView(g_p_vulkan_context,
                                       image,
                                       VK_FORMAT_R8G8B8A8_UNORM,
                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                       VK_IMAGE_VIEW_TYPE_2D, mip_levels);

    image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    info.sampler     = sampler;
    info.imageView   = view;
    info.imageLayout = image_layout;
}

Texture2D::~Texture2D()
{
    vkDestroyImageView(g_p_vulkan_context->_device, view, nullptr);
    vkDestroyImage(g_p_vulkan_context->_device, image, nullptr);
    vkFreeMemory(g_p_vulkan_context->_device, memory, nullptr);
    image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    LOG_INFO("texture2d destroyed {}", name);
}

TextureCube::TextureCube(const std::vector<std::string> &path, const std::string &name, bool gen_mipmap,
                         VkFormat image_format)
{
    assert(path.size() == 6);
    // TextureCube暂时不支持mipmap
    assert(gen_mipmap == false);

    this->name = name;
    this->path = path[0];

    int     texture_width, texture_height, texture_channel;
    stbi_uc *pixels = stbi_load(path[0].c_str(),
                                &texture_width,
                                &texture_height,
                                &texture_channel,
                                STBI_rgb_alpha);


    mip_levels = gen_mipmap ? static_cast<uint32_t>(std::floor(std::log2(std::max(texture_width, texture_height)))) + 1
                            : 1;
    width      = texture_width;
    height     = texture_height;

    VkDeviceSize texture_layer_byte_size;
    switch (image_format)
    {
        case VK_FORMAT_R8G8B8_UNORM:
            texture_layer_byte_size = texture_width * texture_height * 3;
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            texture_layer_byte_size = texture_width * texture_height * 3;
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            texture_layer_byte_size = texture_width * texture_height * 4;
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            texture_layer_byte_size = texture_width * texture_height * 4;
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            texture_layer_byte_size = texture_width * texture_height * 4 * 2;
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            texture_layer_byte_size = texture_width * texture_height * 4 * 3;
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            texture_layer_byte_size = texture_width * texture_height * 4 * 4;
            break;
        default:
            texture_layer_byte_size = VkDeviceSize(-1);
            throw std::runtime_error("invalid texture_layer_byte_size");
    }
    VkDeviceSize cubemap_byte_size = texture_layer_byte_size * 6;

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(g_p_vulkan_context, cubemap_byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer, stagingBufferMemory);

    VkDeviceSize cubemap_offset = 0;

    for (int i = 0; i < 6; i++)
    {
        int image_width, image_height, image_channel;
        pixels = stbi_load(path[i].c_str(),
                           &image_width,
                           &image_height,
                           &image_channel,
                           STBI_rgb_alpha);
        assert(image_width == texture_width);
        assert(image_height == texture_height);
        assert(image_channel == texture_channel);

        void *data;
        vkMapMemory(g_p_vulkan_context->_device, stagingBufferMemory, cubemap_offset, texture_layer_byte_size, 0,
                    &data);
        memcpy(data, pixels, static_cast<size_t>(texture_layer_byte_size));
        free(pixels);
        vkUnmapMemory(g_p_vulkan_context->_device, stagingBufferMemory);
        cubemap_offset += texture_layer_byte_size;
    }
    VulkanUtil::createCubeMapImage(g_p_vulkan_context,
                                   texture_width,
                                   texture_height,
                                   image_format,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                   image, memory, mip_levels);

    VulkanUtil::transitionImageLayout(g_p_vulkan_context,
                                      image,
                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      6, mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);
//
//    cubemap_offset = 0;
//    std::vector<VkBufferImageCopy> bufferCopyRegions;
//
//    VkCommandBuffer commandBuffer = g_p_vulkan_context->beginSingleTimeCommands();
//    for (uint32_t   face          = 0; face < 6; face++)
//    {
//        VkBufferImageCopy region{};
//        region.bufferOffset                    = cubemap_offset;
//        region.bufferRowLength                 = 0;
//        region.bufferImageHeight               = 0;
//        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
//        region.imageSubresource.mipLevel       = 0;
//        region.imageSubresource.baseArrayLayer = face;
//        region.imageSubresource.layerCount     = 1;
//        region.imageOffset                     = {0, 0, 0};
//        region.imageExtent                     = {width, height, 1};
//        bufferCopyRegions.push_back(region);
//        cubemap_offset += texture_layer_byte_size;
//    }
//    vkCmdCopyBufferToImage(commandBuffer,
//                           stagingBuffer,
//                           image,
//                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//                           bufferCopyRegions.size(),
//                           bufferCopyRegions.data());
//
//    g_p_vulkan_context->endSingleTimeCommands(commandBuffer);
//

    VulkanUtil::copyBufferToImage(g_p_vulkan_context,
                                  stagingBuffer,
                                  image,
                                  texture_width,
                                  texture_height,
                                  6);

    VulkanUtil::transitionImageLayout(g_p_vulkan_context,
                                      image,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      6, mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);

    vkDestroyBuffer(g_p_vulkan_context->_device, stagingBuffer, nullptr);
    vkFreeMemory(g_p_vulkan_context->_device, stagingBufferMemory, nullptr);

    sampler = VulkanUtil::getOrCreateCubeMapSampler(g_p_vulkan_context, mip_levels);
    view    = VulkanUtil::createImageView(g_p_vulkan_context,
                                          image,
                                          image_format,
                                          VK_IMAGE_ASPECT_COLOR_BIT,
                                          VK_IMAGE_VIEW_TYPE_CUBE,
                                          mip_levels,0 ,6);

    image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    info.sampler     = sampler;
    info.imageView   = view;
    info.imageLayout = image_layout;
}

TextureCube::~TextureCube()
{
    vkDestroyImageView(g_p_vulkan_context->_device, view, nullptr);
    vkDestroyImage(g_p_vulkan_context->_device, image, nullptr);
    vkFreeMemory(g_p_vulkan_context->_device, memory, nullptr);
    image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    LOG_INFO("texturecube destroyed {}", name);
}

