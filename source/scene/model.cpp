//
// Created by kyros on 5/27/23.
//

#include "scene/model.h"

using namespace Scene;

void Model::Tick()
{
    mesh_loaded->model_matrix = transform.GetTransformMatrix();
}

bool Model::LoadModelFile(const std::string &mesh_path)
{
    Assimp::Importer importer;

    const aiScene *pScene = importer.ReadFile(mesh_path,
                                              aiProcess_Triangulate |
                                              aiProcess_CalcTangentSpace | // 计算uv镜像
                                              aiProcess_ConvertToLeftHanded);
    if (pScene == nullptr)
        return false;
    m_index_count = 0;
    m_submeshes.clear();
    mesh_loaded = std::make_shared<RenderSystem::RenderMesh>();
    processModelNode(pScene->mRootNode, pScene);
    mesh_loaded->m_submeshes = m_submeshes;
    return true;
}

void Model::processModelNode(aiNode *node, const aiScene *scene)
{
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processModelNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
//    mesh_loaded->m_name = mesh->mName.C_Str();
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        RenderSystem::VulkanMeshVertexPostition vertex_position;
        RenderSystem::VulkanMeshVertexNormal    vertex_normal;
        RenderSystem::VulkanMeshVertexTexcoord  vertex_texcoord;

        // 处理顶点位置、法线和纹理坐标
        vertex_position.position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex_normal.normal     = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        Vector3 tangent;
        Vector3 bitangent;

        tangent   = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        bitangent = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        vertex_normal.tangent = Vector4(tangent.x, tangent.y, tangent.z, 1.0f);

        // 计算切线空间手性
        if (vertex_normal.normal.crossProduct(tangent).dotProduct(bitangent) < 0.0f)
        {
            vertex_normal.tangent.w = -1.0f;
        } else
        {
            vertex_normal.tangent.w = 1.0f;
        }

        if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
        {
            vertex_texcoord.texCoord.x = mesh->mTextureCoords[0][i].x;
            vertex_texcoord.texCoord.y = mesh->mTextureCoords[0][i].y;
        } else
            vertex_texcoord.texCoord = Vector2::ZERO;

        mesh_loaded->m_positions.push_back(vertex_position);
        mesh_loaded->m_normals.push_back(vertex_normal);
        mesh_loaded->m_texcoords.push_back(vertex_texcoord);
    }
    // 处理索引
    uint32_t          index_count = 0;
    for (unsigned int i           = 0; i < mesh->mNumFaces; i++)
    {
        aiFace            face = mesh->mFaces[i];
        for (unsigned int j    = 0; j < face.mNumIndices; j++)
        {
            mesh_loaded->m_indices.push_back(face.mIndices[j] + m_index_count);
            index_count++;
        }
    }

    RenderSystem::RenderSubmesh render_submesh;
    render_submesh.index_count   = index_count;
    render_submesh.index_offset  = m_index_count;
    render_submesh.vertex_offset = 0;
    render_submesh.parent_mesh   = mesh_loaded;
    m_index_count += index_count;
    render_submesh.material_index = mesh->mMaterialIndex;

    // 处理材质
    if (mesh->mMaterialIndex >= 0)
    {
//        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
//        loadMaterialTextures(material,
                             aiTextureType_DIFFUSE,
                             "texture_diffuse");
//        mesh_loaded->m_textures.insert(mesh_loaded->m_textures.end(), diffuse_maps.begin(), diffuse_maps.end());
//        // 2. specular maps
//        std::vector<RenderSystem::RenderTexture> specular_maps = loadMaterialTextures(material,
//                                                                                     aiTextureType_SPECULAR,
//                                                                                     "texture_specular");
//        mesh_loaded->m_textures.insert(mesh_loaded->m_textures.end(), specular_maps.begin(), specular_maps.end());
//        // 3. normal maps
//        std::vector<RenderSystem::RenderTexture> normal_maps = loadMaterialTextures(material,
//                                                                                   aiTextureType_HEIGHT,
//                                                                                   "texture_normal");
//        mesh_loaded->m_textures.insert(mesh_loaded->m_textures.end(), normal_maps.begin(), normal_maps.end());
//        // 4. height maps
//        std::vector<RenderSystem::RenderTexture> height_maps = loadMaterialTextures(material,
//                                                                                   aiTextureType_AMBIENT,
//                                                                                   "texture_height");
//        mesh_loaded->m_textures.insert(mesh_loaded->m_textures.end(), height_maps.begin(), height_maps.end());
    }
    m_submeshes.push_back(render_submesh);
}

void Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        ;
//        RenderSystem::RenderTexture texture;
//        texture.texture_type = typeName;
//        texture.texture_path = str.C_Str();
//        mesh_loaded->m_textures.push_back(texture);
    }
}




