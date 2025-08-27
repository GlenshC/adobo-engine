#pragma once
#include <vector>
#include <string>

#include "types.h"
#include "core/entity/ecs.h"
#include "binassets/binasset_stl_read.h"
namespace adobo
{
    struct AdoboSceneEntity;
    struct AdoboScene;
    struct AdoboProj;
    struct AdoboSceneEntityData;
    struct AdoboSceneEntityDataPOD;

    struct AdoboSceneEntity
    {
        char          m_name[32];
        ecs::Entity2D m_id;
        i32           m_tex_index;
        i32           m_subtex_index;

        AdoboSceneEntity(const char *name, ecs::Entity2D id, i32 tex_index, i32 subtex_index);
    };

    struct AdoboScene
    {
        char m_name[32];
        std::vector<AdoboSceneEntity> m_entities;
        
        AdoboScene(const char *name);
        AdoboScene(binassets::AssetData &assets, const char *name, AdoboSceneEntityDataPOD *entities, size_t n_entities);
        void create_entity(binassets::AssetData &assets, const char *name, i32 tex_idx, i32 subtex_idx);
    };

    struct AdoboAssetInfo
    {
        std::string path;
        char name[32];

        AdoboAssetInfo(const std::string &_path, const char *_name);
    };

    struct AdoboProj
    {
        binassets::AssetData         m_assets;
        std::vector<AdoboScene>      m_scenes;
        std::vector<AdoboAssetInfo>  m_atlas_info;
        std::vector<AdoboAssetInfo>  m_shader_info;
        char                         m_name[32];
        std::string                  m_proj_path;
        
        void load_atlas(const char *path, const char *name);
        void load_shader(const char *path, const char *name);
        void save_proj(const char *file_path);
    };
    
    bool save_path_exists(const char *file_path);
    void read_adoboproj(AdoboProj &proj, const char *f_name);
    void write_adoboproj(AdoboProj &proj, const char *name);
    void release_adoboproj(AdoboProj &proj);
}