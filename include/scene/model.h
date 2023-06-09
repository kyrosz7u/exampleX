//
// Created by kyros on 5/27/23.
//

#ifndef XEXAMPLE_MODEL_H
#define XEXAMPLE_MODEL_H

#include "render/resource/render_mesh.h"
#include "render/resource/render_texture.h"
#include "transform.h"
#include <vector>
#include <string>
#include <memory>

using namespace RenderSystem;

namespace Scene
{
    class Model
    {
    public:
        std::string name;
        std::string path;
        Transform   transform;
    public:
        Model()
        {}

        ~Model()
        {}

        void Tick();

        bool LoadModelFile(const std::string &model_path, const std::string &model_name);

        void ToGPU()
        {
            mesh_loaded->ToGPU();
        }

        void SetMeshIndex(uint32_t index)
        {
            mesh_loaded->m_index_in_dynamic_buffer = index;
        }

        [[nodiscard]]inline const Matrix4x4 &GetModelMatrix() const
        {
            return model_matrix;
        }

        [[nodiscard]]inline const Matrix4x4 &GetNormalMatrix() const
        {
            return normal_matrix;
        }

        [[nodiscard]]inline const std::vector<RenderSystem::RenderSubmesh> &GetSubmeshes() const
        {
            return m_submeshes;
        }

        [[nodiscard]]inline const std::vector<Texture2DPtr> &GetTextures() const
        {
            return textures_loaded;
        }

    private:
        void processModelNode(aiNode *node, const aiScene *scene);

        void processMesh(aiMesh *mesh, const aiScene *scene);

        uint32_t                                 m_index_count{0};
        Matrix4x4                                model_matrix  = Matrix4x4::IDENTITY;
        Matrix4x4                                normal_matrix = Matrix4x4::IDENTITY;
        std::vector<RenderSystem::RenderSubmesh> m_submeshes;
        RenderMeshPtr                            mesh_loaded;
        std::vector<Texture2DPtr>                textures_loaded;

        void loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

        void clearInternalState();
    };
}
#endif //XEXAMPLE_MODEL_H
