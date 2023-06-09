//
// Created by kyros on 5/21/23.
//

#ifndef XEXAMPLE_DEFER_LIGHT_H
#define XEXAMPLE_DEFER_LIGHT_H

#include "subpass_base.h"
#include "render/resource/render_mesh.h"

using namespace VulkanAPI;

namespace RenderSystem
{
    namespace SubPass
    {
        struct DeferLightPassInitInfo : public SubPassInitInfo
        {
            ImageAttachment *gbuffer_color_attachment;
            ImageAttachment *gbuffer_normal_attachment;
            ImageAttachment *gbuffer_position_attachment;
        };

        class DeferLightPass : public SubPassBase
        {
        public:
            enum _mesh_defer_lighting_pipeline_layout_define
            {
                _mesh_defer_lighting_pass_ubo_data_layout = 0,
                _mesh_defer_lighting_pass_gbuffer_layout,
                _mesh_defer_lighting_pass_directional_light_shadow_layout,
                _mesh_defer_lighting_pass_pipeline_layout_count
            };

            DeferLightPass()
            {
                name = "mesh_defer_light_subpass";
                m_descriptor_set_layouts.resize(_mesh_defer_lighting_pass_pipeline_layout_count);
            }

            void draw() override;

            void updateAfterSwapchainRecreate() override;

        private:
            void initialize(SubPassInitInfo *subPassInitInfo) override;

            void setupPipeLineLayout();

            void setupDescriptorSet() override;

            void setupPipelines() override;

            VkDescriptorSet m_scence_ubo_descriptor_set = VK_NULL_HANDLE;
            VkDescriptorSet m_gbuffer_descriptor_set    = VK_NULL_HANDLE;

            ImageAttachment *m_p_gbuffer_color_attachment    = nullptr;
            ImageAttachment *m_p_gbuffer_normal_attachment   = nullptr;
            ImageAttachment *m_p_gbuffer_position_attachment = nullptr;

            void updateGlobalDescriptorSet();

            void updateGBufferDescriptorSet();
        };
    }
}
#endif //XEXAMPLE_MESH_FORWARD_LIGHT_H
