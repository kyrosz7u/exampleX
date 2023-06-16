//
// Created by kyros on 5/21/23.
//

#ifndef XEXAMPLE_MESH_FORWARD_LIGHT_H
#define XEXAMPLE_MESH_FORWARD_LIGHT_H

#include "subpass_base.h"
#include "render/resource/render_mesh.h"

using namespace VulkanAPI;

namespace RenderSystem
{
    namespace SubPass
    {
        struct DirectionalLightShadowPassInitInfo : public SubPassInitInfo
        {
            std::vector<RenderMeshPtr> *render_mesh_list = nullptr;
        };

        class MeshPass : public SubPassBase
        {
        public:
            enum _mesh_pass_pipeline_layout_define
            {
                _mesh_pass_ubo_data_layout = 0,
                _mesh_pass_texture_layout,
                _mesh_pass_pipeline_layout_count
            };
            MeshPass()
            {
                name = "mesh_subpass";
            }

            void draw() override;

            void updateGlobalRenderDescriptorSet();

            void updateAfterSwapchainRecreate() override;

        private:
            void initialize(SubPassInitInfo *subPassInitInfo) override;
            void setupPipeLineLayout();
            void setupDescriptorSet() override;
            void setupPipelines() override;
            VkDescriptorSet m_mesh_light_ubo_descriptor_set = VK_NULL_HANDLE;
        };
    }
}
#endif //XEXAMPLE_MESH_FORWARD_LIGHT_H