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

        std::vector<RenderSystem::RenderSubmesh> getSubmeshes()
        {
            return m_submeshes;
        }

        std::vector<Texture2DPtr> getTextures()
        {
            return textures_loaded;
        }

    private:
        void processModelNode(aiNode *node, const aiScene *scene);

        void processMesh(aiMesh *mesh, const aiScene *scene);

        uint32_t                                 m_index_count{0};
        std::vector<RenderSystem::RenderSubmesh> m_submeshes;
        RenderMeshPtr                            mesh_loaded;
        std::vector<Texture2DPtr>                textures_loaded;

        void loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

        void clearInternalState();
    };
}
#endif //XEXAMPLE_MODEL_H
