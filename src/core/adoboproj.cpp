#include "types.h"
#include "binassets/binasset_stl_read.h"
#include "core/entity/ecs.h"
#include "util/debug.h"

#include <cstring>
#include <unordered_map>
#include <algorithm>
#include "core/adoboproj.h"

#include <filesystem>

#define MAX_ALLOWED_SIZE (64 * 1024 * 1024) // 64MB

// for writing
namespace adobo
{
    struct ProjMetadata{
        i32 n_atlas;
        i32 n_shader;
        i32 n_scenes;
        i32 n_entities;
        i32 n_total_path_size;
    };

    struct AdoboSceneEntityDataPOD
    {
        char name[32];
        adobo::vec3f position;
        adobo::vec2f scale;
        adobo::vec3f rotation;
        i32 tex_index;
        i32 subtex_index;
        i32 type;
    };

    struct AdoboSceneEntityData
    {
        char         name[32];
        adobo::vec3f position;
        adobo::vec2f scale;
        adobo::vec3f rotation;
        i32          tex_index;
        i32          subtex_index;
        i32          type;

        AdoboSceneEntityData(
            const char *_name, 
            const adobo::vec3f &_position, 
            const adobo::vec2f &_scale,
            const adobo::vec3f &_rotation,
            const i32 _tex_index,
            const i32 _subtex_index,
            const i32 _type
        )
            : position(_position), scale(_scale), rotation(_rotation),
              tex_index(_tex_index), subtex_index(_subtex_index),
              type(_type) 
        {
            std::strncpy(name, _name, sizeof(name));
            name[sizeof(name) - 1] = '\0'; // Ensure null-termination
        } 
    };
    

    AdoboScene::AdoboScene(const char *name)
    {
        std::strncpy(m_name, name, sizeof(m_name));
        m_name[sizeof(m_name) - 1] = '\0'; // Ensure null-termination
    }

    AdoboScene::AdoboScene(
        bsst::AssetData &assets, 
        const char *name, 
        AdoboSceneEntityDataPOD *entities, 
        size_t n_entities
    )
    {
        std::strncpy(m_name, name, sizeof(m_name));
        m_name[sizeof(m_name) - 1] = '\0'; // Ensure null-termination

        m_entities.reserve(n_entities);
        for (size_t i = 0; i < n_entities; i++)
        {
            auto &e = entities[i];
            ecs::Entity2D ent_id;
            auto ent_data     = ecs::create(ent_id);
            ent_data.position = e.position;
            ent_data.scale    = e.scale;
            ent_data.rotation = e.rotation;
            ent_data.tex      = assets.atlases[e.tex_index].tex;
            ent_data.tex_uv   = assets.atlases[e.tex_index].tex[e.subtex_index];
            ent_data.type     = e.type;
            m_entities.emplace_back(e.name, ent_id, e.tex_index, e.subtex_index);
        }
    }
    void AdoboScene::create_entity(binassets::AssetData &assets, const char *name, i32 tex_idx, i32 subtex_idx)
    {
        ecs::Entity2D ent;
        auto data = ecs::create(ent);
        data.aabb = nullptr;
        data.position = {0.5f, 0.5f, 0.0f};
        data.scale = {0.2f, 0.2f};
        data.rotation = {};
        data.tex = assets.atlases[tex_idx].tex;
        data.tex_uv = assets.atlases[tex_idx].tex[subtex_idx];
        data.type = 0;

        m_entities.emplace_back(name, ent, tex_idx, subtex_idx);

        DEBUG_LOG("Editor: Created entity %s\n", name);
    }

    AdoboSceneEntity::AdoboSceneEntity(const char *name, ecs::Entity2D id, i32 tex_index, i32 subtex_index)
        : m_id(id), m_tex_index(tex_index), m_subtex_index(subtex_index)
    {
        std::strncpy(m_name, name, sizeof(m_name));
        m_name[sizeof(m_name) - 1] = '\0';
    }

    AdoboAssetInfo::AdoboAssetInfo(const std::string &_path, const char *_name)
        : path(_path)
    {
        std::strncpy(name, _name, sizeof(name));
        name[sizeof(name) - 1] = '\0'; // Ensure null-termination
    }

    void AdoboProj::load_atlas(const char *path, const char *name)
    {
        if (!path || !name)
        {
            DEBUG_ERR("PROJ_ERR: PATH OR NAME IS NULL\n");
            return;
        }
        if (std::strlen(name) == 0)
        {
            DEBUG_ERR("PROJ_ERR: ASSET NO NAME\n");
            return;
        }
        if (bsst::assets_load_atlas(m_assets, path, name))
        {
            DEBUG_ERR("PROJ_ERR: FAILED TO LOAD ATLAS\n");
            return;
        }
        m_atlas_info.emplace_back(path, name);


    }
    void AdoboProj::load_shader(const char *path, const char *name)
    {
        if (!path || !name)
        {
            DEBUG_ERR("PROJ_ERR: PATH OR NAME IS NULL\n");
            return;
        }
        if (std::strlen(name) == 0)
        {
            DEBUG_ERR("PROJ_ERR: ASSET NO NAME\n");
            return;
        }
        if (bsst::assets_load_shader(m_assets, path, name))
        {
            DEBUG_ERR("PROJ_ERR: FAILED TO LOAD SHADER\n");
            return;
        }
        m_shader_info.emplace_back(path, name);
    }
    void AdoboProj::save_proj(const char *file_path)
    {
        write_adoboproj(*this, file_path);
    }

    bool save_path_exists(const char *file_path)
    {
        return std::filesystem::exists(file_path);
    }
    
    void release_adoboproj(AdoboProj &proj)
    {
        bsst::assets_free(proj.m_assets);
        proj.m_atlas_info.clear();
        proj.m_shader_info.clear();
        proj.m_scenes.clear();
        proj.m_name[0] = 0;
    }


    void read_adoboproj(AdoboProj &proj, const char *f_name)
    {
        DEBUG_LOG("ADOBOPROJ: READING PROJECT %s\n", f_name);

        const char header[] = "adoboproj";

        auto file_size = std::filesystem::file_size(f_name);
        if (file_size > MAX_ALLOWED_SIZE)
        {
            DEBUG_ERR("YOUR MOM SO FAT LIKE THE FILE YOU TRIED TO OPEN.\n");
            return;
        }

        std::FILE *file = std::fopen(f_name, "rb");   
        if (!file)
        {
            DEBUG_ERR("FAILED TO OPEN FILE\n");
            return;
        }

        // Read header
        char header_buffer[sizeof(header)];
        if (std::fread(header_buffer, 1, sizeof(header_buffer), file) != sizeof(header_buffer)) 
        {
            DEBUG_ERR("FAILED TO READ HEADER\n");
            std::fclose(file);
            return;
        }

        // Compare header safely (binary safe)
        if (std::memcmp(header_buffer, header, sizeof(header_buffer)) != 0) 
        {
            DEBUG_ERR("WRONG FILE HEADER\n");
            std::fclose(file);
            return;
        }

        std::size_t payload_size = file_size - sizeof(header);
        u8 *mem = (u8*)std::malloc(payload_size);
        if (!mem) {
            DEBUG_ERR("FAILED TO ALLOCATE MEMORY\n");
            std::fclose(file);
            return;
        }

        // read remaining bytes directly
        if (std::fread(mem, 1, payload_size, file) != payload_size) {
            DEBUG_ERR("FAILED TO READ PAYLOAD\n");
            std::free(mem);
            std::fclose(file);
            return;
        }

        proj.m_proj_path = f_name;
        DEBUG_LOG("PROJ PATH: %s\n", proj.m_proj_path.c_str());

        size_t boffset = 0;
        ProjMetadata *counts     = nullptr;
        i32  *num_scene_ent      = nullptr;
        i32  *atlas_path_size    = nullptr;
        i32  *shader_path_size   = nullptr;
        char *project_name       = nullptr;
        char *buffer_scene_names = nullptr;
        AdoboSceneEntityDataPOD *buffer_ents = nullptr;
        
        char *buffer_atlas_name  = nullptr;
        char *buffer_shader_name = nullptr;
        char *buffer_atlas_path  = nullptr;
        char *buffer_shader_path = nullptr;

        counts =            (ProjMetadata *)mem;
        num_scene_ent =     (i32 *) (mem + (boffset += sizeof(ProjMetadata)));
        atlas_path_size =   (i32 *) (mem + (boffset += sizeof(i32) * counts->n_scenes));
        shader_path_size =  (i32 *) (mem + (boffset += sizeof(i32) * counts->n_atlas));
        project_name =      (char *)  (mem + (boffset += sizeof(i32) * counts->n_shader));
        
        buffer_scene_names = (char *) (mem + (boffset += 32));
        buffer_ents        = (AdoboSceneEntityDataPOD *) (mem + (boffset += counts->n_scenes * 32));
        buffer_atlas_name  = (char *) (mem + (boffset += counts->n_entities * sizeof(AdoboSceneEntityDataPOD)));
        buffer_shader_name = (char *) (mem + (boffset += counts->n_atlas * 32));
        buffer_atlas_path  = (char *) (mem + (boffset += counts->n_shader * 32));

        strncpy(proj.m_name, project_name, 32);
        proj.m_name[31] = 0;

        size_t path_offset = 0;
        for (i32 i =0; i < counts->n_atlas; i++)
        {
            proj.load_atlas(buffer_atlas_path + path_offset, buffer_atlas_name + (i * 32));
            path_offset += atlas_path_size[i] + 1;
        }
        
        buffer_shader_path = buffer_atlas_path + path_offset; 
        path_offset = 0;

        for (i32 i = 0; i < counts->n_shader; i++)
        {
            proj.load_shader(buffer_shader_path + path_offset, buffer_shader_name + (i * 32));
            path_offset += shader_path_size[i] + 1;
        }

        for (i32 i = 0; i < counts->n_scenes; i++)
        {
            proj.m_scenes.emplace_back(
                proj.m_assets, 
                buffer_scene_names + (i * 32), 
                buffer_ents, num_scene_ent[i]
            ); 
        }

        fclose(file);
        free(mem);
        DEBUG_LOG("ADOBOPROJ: SUCCESSFUL READING %s\n", f_name);
    }

    void write_adoboproj(AdoboProj &proj, const char *f_name)
    {
        DEBUG_LOG("ADOBOPROJ: WRITING PROJECT %s\n", f_name);
        const char header[] = "adoboproj";
        std::string tempname = std::string("_tempfile_.adoboproj");

        std::FILE *file = std::fopen(tempname.c_str(), "wb");
        if (!file)
        {
            DEBUG_ERR("WRITE_ADOBOPROJ: CAN'T CREATE OR OPEN FILE %s\n", tempname.c_str());
            return;
        }

        proj.m_proj_path = f_name;

        ProjMetadata counts = {
            (i32) proj.m_assets.atlases.size(), 
            (i32) proj.m_assets.shaders.size(), 
            (i32) proj.m_scenes.size(),
            0,
            0
        };

        for (auto &s : proj.m_atlas_info)
        {
            counts.n_total_path_size += s.path.size() + 1;
        }

        for (auto &s : proj.m_shader_info)
        {
            counts.n_total_path_size += s.path.size() + 1;
        }
        for (auto &s : proj.m_scenes)
        {
            counts.n_entities += s.m_entities.size();
        }

        size_t file_size = 
            sizeof(i32) * (counts.n_atlas + counts.n_shader + counts.n_scenes) + // n_entities + atlas and shader path sizes
            32 + // project name
            counts.n_total_path_size + // atlas shader path
            32 * (counts.n_atlas + counts.n_shader) + // atlas shader name
            counts.n_scenes * 32 + // scene names
            counts.n_entities * sizeof(AdoboSceneEntityData)
        ;
        
        // size_t mem_offset = 0;
        u8 *baseptr = (u8 *)malloc(file_size);
        u8 *mem = baseptr;
        
        for (auto &s : proj.m_scenes)
        {
            int val = (i32)s.m_entities.size();
            memcpy(mem, &val, sizeof(i32));
            mem += sizeof(i32);
        }

        for (auto &s : proj.m_atlas_info)
        {
            int val = (i32)s.path.size();
            memcpy(mem, &val, sizeof(i32));
            mem += sizeof(i32);
        }

        for (auto &s : proj.m_shader_info)
        {
            int val = (i32)s.path.size();
            memcpy(mem, &val, sizeof(i32));
            mem += sizeof(i32);
        }

        memcpy(mem, proj.m_name, 32);
        mem += 32;

        for (auto &s : proj.m_scenes)
        {
            memcpy(mem, s.m_name, 32);
            mem += 32;
        }

        for (auto &s : proj.m_scenes)
        {
            for (auto &e : s.m_entities)
            {
                AdoboSceneEntityDataPOD ent;
                
                auto data = e.m_id();
                
                memcpy(ent.name, e.m_name, 32);
                ent.position = data.position;
                ent.scale = data.scale;
                ent.rotation = data.rotation;
                ent.tex_index    = e.m_tex_index;
                ent.subtex_index = e.m_subtex_index;
                ent.type = data.type;
                
                memcpy(mem, &ent, sizeof(AdoboSceneEntityDataPOD));
                mem += sizeof(AdoboSceneEntityDataPOD);
            }
        }

        // name
        for (auto &s : proj.m_atlas_info)
        {
            memcpy(mem, s.name, 32);
            mem += 32;
        }

        for (auto &s : proj.m_shader_info)
        {
            memcpy(mem, s.name, 32);
            mem += 32;
        }

        // path
        for (auto &s : proj.m_atlas_info)
        {
            memcpy(mem, s.path.data(), s.path.size()); 
            mem += s.path.size();
            *(mem++) = 0;
        }

        for (auto &s : proj.m_shader_info)
        {
            memcpy(mem, s.path.data(), s.path.size()); 
            mem += s.path.size();
            *(mem++) = 0;
        }

        size_t payload_size = 0;
        if (mem > baseptr)  payload_size = mem - baseptr;
        else                payload_size = baseptr - mem;

        fwrite(header, sizeof(header), 1, file);
        fwrite(&counts, sizeof(ProjMetadata), 1, file);
        fwrite(baseptr, 1, payload_size, file);

        fclose(file);
#ifdef _WIN32
        std::remove(f_name);
#endif
        if (rename(tempname.c_str(), f_name))
        {
            DEBUG_ERR("ERROR: RENAMING TEMPFILE TO FINAL FILE");
            return;
        }
        // DEBUG_LOG("Successfully writen adoboproj %s.\n", f_name);
        DEBUG_LOG("ADOBOPROJ: SUCCESSFUL WRITE %s\n", f_name);
    }
}