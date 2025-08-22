#include "imgui.h"
#include "types.h"

#include "game/editor.h"
#include "core/entity/ecs.h"
#include "renderer/sprite2D.h"
#include "binassets/binasset_json.h"
#include "binassets/binasset_read.h"
#include "aiekick/ImGuiFileDialog.h"
#include "core/platform.h"
#include "cglm/cglm.h"
#include "renderer/shader.h"

#include <cstring>
#include <vector>
#include <string>

#define OPTDEF(x, de) ((x) ? (x) : (de))

namespace editor
{
    struct TexNames
    {
        std::string  ent;
        char        *tex;
        char        *tex_uv;
    };

    struct Scene
    {
        std::vector<ecs::Entity2D> entities;
        std::vector<TexNames>      names;
        std::string assets_file;
        std::string names_json_file;

        void create_entity(const char *name, const texture::Texture &tex, const adobo::vec4f &tex_uv)
        {
            ecs::Entity2D ent;
            auto data = ecs::create(ent);
            data.position = {0.25f, 0.25f};
            data.scale = {0.5f,0.5f};
            data.rotation = {};
            data.tex    = tex;
            data.tex_uv = tex_uv;

            entities.push_back(ent);
            names.push_back({name,nullptr,nullptr});

            DEBUG_LOG("Editor: Created entity %s\n", name);
        }

        size_t size()
        {
            return entities.size();
        }
    };

    static Scene scene;
    static size_t              sel_index      = 0;
    static texture::Texture    sel_tex        = {-1};
    static i32                 sel_tex_uv_idx     = 0;

    static char                ent_name[32]   = "New Entity";
    static char              **file_ptr       = nullptr;
    static char               *glnsh_file     = nullptr;
    static char               *names_file     = nullptr;
    static mat4                u_projection   = GLM_MAT4_IDENTITY_INIT;
    
    static shader::Shader      editor_shader;
    
    bsst::AssetData  editor_assets = {};
    bsst::AssetNames editor_asset_names = {};

    void file_dialog_gui();

    void load_assets()
    {
        bsst::assets_free(editor_asset_names);
        bsst::assets_free(editor_assets);

        // bsst::assets_free_on_load(true);
        bsst::assets_load_bin_s(editor_assets, glnsh_file);
        bsst::assets_upload_atlases(editor_assets);

        bsst::assets_load_json(editor_asset_names, names_file);
    }


    void editor_init()
    {
        glm_mat4_identity(u_projection);
        glm_ortho(0, 1.0f, 1.0f, 0, -1.0f, 1.0f, u_projection);
        shader::create(editor_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");

        shader::bind(editor_shader);
        shader::set_uniform_mat4(editor_shader, "u_projection", u_projection);
    }

    void editor_render()
    {
        shader::bind(editor_shader);
        shader::set_uniform_mat4(editor_shader, "u_projection", u_projection);

        renderer::begin_sprites(editor_shader);
        for (auto &e : scene.entities)
        {
            renderer::submit_sprites(e);
        }
        renderer::end_sprites();
    }

    void editor_gui()
    {
        static bool show_file_required = false;
        file_dialog_gui();

        ImGui::Begin("Scene Browser");

        ImGui::Separator();

        ImGui::Text("Open Asset");

        ImGui::Text("GLNSH FILE: %s", (glnsh_file) ? (glnsh_file) : ("(NONE)"));
        ImGui::SameLine();

        if (ImGui::Button("Choose glnsh file"))
        {
            file_ptr = &glnsh_file;
            IGFD::FileDialogConfig config;
            config.path = "./assets";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose .glnsh file", ".glnsh", config);
        }

        ImGui::Text("JSON FILE : %s", (names_file) ? (names_file) : ("(NONE)"));
        ImGui::SameLine();
        if (ImGui::Button("Choose json file"))
        {
            file_ptr = &names_file;
            IGFD::FileDialogConfig config;
            config.path = "./assets";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose .json file", ".json", config);
        }

        if (ImGui::Button("Load assets"))
        {
            if (glnsh_file && names_file)
            {
                load_assets();
            }
            else
            {
                show_file_required = true;
            }
        }
        if (show_file_required)
        {
            ImGui::Text("GLNSH and JSON file is required");   
        }

        ImGui::Separator();
       
        ImGui::Text("Entity Name:");
        ImGui::SameLine();
        ImGui::InputText("##ent_name", ent_name, IM_ARRAYSIZE(ent_name));

        if (ImGui::Button("Create Entity"))
        {
            if (editor_assets.atlases && texture::is_valid(sel_tex) && sel_tex_uv_idx >= 0)
            {
                scene.create_entity(ent_name, sel_tex, sel_tex[sel_tex_uv_idx]);
            }

        }

        ImGui::Separator();

        ImGui::Text("Entities:");

        for (size_t i = 0; i < scene.entities.size(); i++)
        {
            bool is_selected = (sel_index == i);
            if (ImGui::Selectable(scene.names[i].ent.c_str(), is_selected))
            {
                sel_index = i;
            }
        }

        ImGui::End();

        // === Inspector ===
        ImGui::Begin("Inspector");

        if (!scene.entities.empty() && scene.entities.size() > sel_index )
        {
            ImGui::Text("Entity: %s", scene.names[sel_index].ent.c_str());
            
            auto data = scene.entities[sel_index]();
            ImGui::InputFloat2("Position", data.position);
            ImGui::InputFloat2("Scale", data.scale);
            ImGui::InputFloat3("Rotation",    data.rotation);
            ImGui::Text("Texture: %s" , OPTDEF(scene.names[sel_index].tex, "(NONE)"));
            ImGui::Text("Subtex: %s", OPTDEF(scene.names[sel_index].tex_uv, "(NONE)"));
        }
        else
        {
            ImGui::Text("No entity selected.");
        }

        ImGui::End();


        ImGui::Begin("Asset Browser");

        for (size_t i = 0; i < editor_assets.atlases_size; i++)
        {

            auto &atlases = editor_assets.atlases;
            auto &atlas_names = editor_asset_names.atlases;
            bool is_selected = (atlases[i].tex == sel_tex);

            if (ImGui::Selectable(atlas_names[i].name, is_selected)) 
            {
                if (sel_tex == atlases[i].tex)
                {
                    sel_tex.id = -1; 
                    sel_tex_uv_idx = -1;
                }
                else
                {
                    sel_tex = atlases[i].tex;
                    sel_tex_uv_idx = -1;
                }
                
            }
            if (is_selected)
            {
                ImGui::Indent();
                for (i32 j = 0; j < atlases[i].subtex_n; j++)
                {
                    bool is_subselected = (j == sel_tex_uv_idx);
                    if (ImGui::Selectable(atlas_names[i].subtex[j].name, is_subselected))
                    {
                        if (is_subselected)
                        {
                            sel_tex_uv_idx = -1;
                        }
                        else
                        {
                            sel_tex_uv_idx = j;
                        }
                    }
                }
                ImGui::Unindent();
            }          
        }

        ImGui::End();
    }

    void file_dialog_gui()
    {   
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
                *file_ptr = (char *)malloc(path.size() + 1);
                memcpy(*file_ptr, path.data(), path.size());
                (*file_ptr)[path.size()] = 0;
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

}