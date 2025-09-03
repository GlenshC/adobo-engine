#include <glad/gl.h>

#include "imgui.h"
#include "types.h"

// #include "binassets/binasset_json.h"
// #include "binassets/binasset_read.h"
#include "binassets/binasset_stl_read.h"
#include "aiekick/ImGuiFileDialog.h"
#include "cglm/cglm.h"

#include "core/entity/ecs.h"
#include "core/entity/hitbox.h"
#include "core/platform.h"
#include "renderer/sprite2D.h"
#include "renderer/shader.h"
#include "core/editor.h"

#include <cstring>
#include <vector>
#include <string>

#define OPTDEF(x, de) ((x) ? (x) : (de))
#define OPTDEFPTR(a, b, de) ((a) ? (OPTDEF(b, de)) : (de))

namespace editor
{
    std::vector<adobo::vec4f> draw_rect_buffer;
    bool is_valid(const i32 tex_index);
    bool is_valid(const i32 tex_index,const i32 subtex_index);
    bool is_valid_ent(adobo::AdoboScene *scene, i32 ent_index);
    bool is_valid_ent(i32 ent_index);

    struct EditorState
    {
        ecs::HitboxType hitbox_type;
        i32     sel_ent_index;
        i32     sel_hitbox_index;
        i32     sel_subhitbox_index;
        i32     sel_tex_index;
        i32     sel_subtex_index;
        i32     sel_ent_scene_index;
        
        i32     active_scene_index;
        char    buffer_ent[32];
        char    buffer_scene[32];
    };

    EditorState g_state = {ecs::HITBOX_TYPE_AABB, 0, -1, -1, -1, -1, -1, 0, "\0", "\0"};
    
    static mat4                u_projection   = GLM_MAT4_IDENTITY_INIT;
    static shader::Shader      editor_shader;

    adobo::AdoboProj project = {};
    
    
    void scene_create_entity();
    void proj_create_hitbox();

    void draw_ent_hitbox(ecs::HitboxAABB &aabb, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale);
    void draw_begin_hitbox();
    void draw_submit_ent_hitbox(ecs::HitboxAABB &aabb, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale);
    void draw_end_hitbox();

    void file_dialog_gui();
    void gui_entity_list();
    void gui_entity_inspector();
    void gui_section_hitbox_editor();
    void gui_asset_browser();

    u8 component_uniform_dragf2(const char*name, f32 *v, u8 *state);
    bool component_input_int(const char* label, int *v, int min, int max, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    void button_save_proj();
    void button_load_asset();
    void button_load_proj();

    void gui_section_hitbox_editor()
    {
    }

    void editor_init()
    {
        glm_mat4_identity(u_projection);
        glm_ortho(0, 1.0f, 1.0f, 0, -1.0f, 1.0f, u_projection);
        shader::create(editor_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");

        shader::bind(editor_shader);
        shader::set_uniform_mat4(editor_shader, "u_projection", u_projection);
        project.m_scenes.emplace_back("scene1");
        g_state.active_scene_index = 0;
    }
    void editor_save()
    {
        if (adobo::save_path_exists(project.m_proj_path.c_str()))
        {
            project.save_proj(project.m_proj_path.c_str());
        }
        else
            button_save_proj();
    }

    void editor_gui()
    {
        file_dialog_gui();
        
        gui_entity_list();        
        gui_entity_inspector();
        gui_asset_browser();
    }

    void editor_render()
    {
        // TODO entity layer moving
        adobo::AdoboScene *scene = nullptr;
        shader::bind(editor_shader);
        shader::set_uniform_mat4(editor_shader, "u_projection", u_projection);

        draw_begin_hitbox();
        renderer::begin_sprites(editor_shader);
        if ((scene = proj_active_scene()))
        {
            for (auto &e : scene->m_entities)
            {
                renderer::submit_sprites(e.m_id);
                ecs::Hitbox hbid = e.m_id.get_val<ecs::Tag_Hitbox>();
                if (hbid.is_valid())
                {
                    auto &hb = hbid.get<ecs::HitboxUnion>();
                    if (hb.type & ecs::HITBOX_TYPE_AABB)
                        draw_submit_ent_hitbox(hb.aabb, e.m_id.get<ecs::Tag_Position>(), e.m_id.get<ecs::Tag_Scale>());
                }
            }
        }
        renderer::end_sprites();
        draw_end_hitbox();
    }
    
    void scene_create_entity()
    {
        if (g_state.buffer_ent[0] == 0) { DEBUG_ERR("NO NAME SPECIFIED\n"); return; }
        if (!project.m_assets.atlases.size()) { DEBUG_ERR("NO ASSET LOADED\n"); return; }
        if (!is_valid(g_state.sel_tex_index, g_state.sel_subtex_index)) { DEBUG_ERR("NO SHIT DAWG BUT YOU AINT SELECTING A VALID TEXTURE\n"); return; }
        if (!texture::is_valid(project.m_assets.atlases.at(g_state.sel_tex_index).tex, g_state.sel_subtex_index)) { DEBUG_ERR("INVALID TEXTURE BIH\n"); return; }

        adobo::AdoboScene *scene = nullptr;
        if ((scene = proj_active_scene()))
        {
            scene->create_entity(
                project.m_assets,
                g_state.buffer_ent,
                g_state.sel_tex_index,
                g_state.sel_subtex_index
            );
            g_state.buffer_ent[0] = 0;
        }
    }

    void proj_create_hitbox()
    {
        adobo::AdoboScene *scene = proj_active_scene();
        if (is_valid_ent(scene, g_state.sel_ent_index))
        {
            auto &ent = scene->m_entities[g_state.sel_ent_index];
            ecs::Hitbox &ent_hitbox = ent.m_id.get<ecs::Tag_Hitbox>();
            
            if (g_state.hitbox_type == ecs::HITBOX_TYPE_AABB)
            {
                DEBUG_LOG("Creating HitboxAABB\n");
                ecs::HitboxAABB &aabb = ecs::create_hitbox<ecs::HITBOX_TYPE_AABB>(ent_hitbox);

                aabb.emplace_back(0, 0, 1.0f, 1.0f);
                DEBUG_LOG("Created HitboxAABB\n");
            } 
            else
            {
                DEBUG_LOG("Creating HitboxCircle\n");
                ecs::HitboxCircle &circle = ecs::create_hitbox<ecs::HITBOX_TYPE_CIRCLE>(ent_hitbox);
                
                circle.emplace_back(0, 0, 1.0f);
                DEBUG_LOG("Created HitboxCircle\n");
            }
            ent.m_hitbox_index = project.m_hitboxes.size();
            g_state.sel_hitbox_index = (i32)project.m_hitboxes.size();
            g_state.sel_subhitbox_index = 0;
            project.m_hitboxes.push_back(ent_hitbox);
        }
    }

    void draw_ent_hitbox(ecs::HitboxAABB &aabb, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale)
    {
        adobo::vec4f rect;
        adobo::vec2f ent_topleft = {ent_pos.x - (0.5f * ent_scale.x), ent_pos.y - (0.5f * ent_scale.y)};

        for (int i = 0; i < aabb.size; i++)
        {
            rect.x = ent_topleft.x + aabb.data[i].x * ent_scale.x;
            rect.y = ent_topleft.y + aabb.data[i].y * ent_scale.y;
            rect.z = aabb.data[i].z * ent_scale.x;
            rect.w = aabb.data[i].w * ent_scale.y;
            
            renderer::draw_shape_rect(rect, u_projection);
        }
        // glEnable(GL_DEPTH_TEST);
    }

    void draw_begin_hitbox()
    {
        draw_rect_buffer.clear();
    }

    void draw_submit_ent_hitbox(ecs::HitboxAABB &aabb, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale)
    {
        adobo::vec4f rect;

        // Parent top-left (because ent_pos is center)
        float parent_tl_x = ent_pos.x - 0.5f * ent_scale.x;
        float parent_tl_y = ent_pos.y - 0.5f * ent_scale.y;

        for (int i = 0; i < aabb.size; i++)
        {
            // Position relative to parent top-left
            rect.x = parent_tl_x + aabb.data[i].x * ent_scale.x;
            rect.y = parent_tl_y + aabb.data[i].y * ent_scale.y;

            // Size relative to parent size
            rect.z = aabb.data[i].z * ent_scale.x;
            rect.w = aabb.data[i].w * ent_scale.y;

            draw_rect_buffer.emplace_back(rect);
        }
    }

    void draw_end_hitbox()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (auto &e : draw_rect_buffer)
        {
            renderer::draw_shape_rect(e, u_projection);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void proj_create_scene()
    {
        g_state.buffer_scene[31] = 0; 
        if (g_state.buffer_scene[0] != 0 || strlen(g_state.buffer_scene))
        {
            i32 index = (i32)project.m_scenes.size();
            project.m_scenes.emplace_back(g_state.buffer_scene);
            g_state.active_scene_index = index;
            g_state.buffer_scene[0] = 0;
        }
    }

    void gui_popup_confirm_delete()
    {
        adobo::AdoboScene *scene = proj_active_scene();
        if (ImGui::BeginPopupModal("ConfirmSceneDeletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to delete this?");
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                // >>> Perform delete action here
                if (scene)
                {
                    project.m_scenes.erase(project.m_scenes.begin() + g_state.active_scene_index);
                    g_state.active_scene_index = 0;
                }
                
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }


    /**************************************** 
     *                                      *
     * GUIS                                 *
     *                                      *
     ****************************************/


    bool component_input_int(const char* label, int *v, int min, int max, int step, int step_fast, ImGuiInputTextFlags flags)
    {
        if (ImGui::InputInt(label, v, step, step_fast, flags))
        {
            if (*v >= max)
                *v = max - 1;
            if (*v < min)
                *v = min;
            return true;
        }
        return false;
    }
    
    // before using set, static f32 locked_aspect = 1.0
    // returns interact flags: 1 (v.x), 2(v.y), 4(checkbox)
    u8 component_uniform_dragf2(const char*name, f32 *v, u8 *state)
    {
        // state 1 = uniform_scaling, state 2 = aspect_locked
        static f32 locked_aspect = 1.0;
        std::string pid = (std::to_string(reinterpret_cast<uintptr_t>(state)));

        ImGui::PushID(pid.c_str()); // unique scope
        u8 flag = 0;

        // WIDTH_CALCULATION
        float totalWidth = ImGui::CalcItemWidth();
        float checkboxWidth = ImGui::GetFrameHeight(); // square checkbox
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        float itemWidth = (totalWidth - checkboxWidth - spacing * 2.0f) / 2.0f;

        ImGui::PushItemWidth(itemWidth);
        if (ImGui::DragFloat((std::string("##x") + pid).c_str(), &v[0], 0.0001f, -1.0f, 2.0f, "%f"))
        {
            flag |= 1;
            if (ImGui::IsItemActive() && (*state & 1))
            {
                if (!(*state & 2))
                {
                    locked_aspect = v[0] / v[1];
                    *state |= 2; // aspect_locked = true
                }
                v[1] = v[0] / locked_aspect;
            }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, spacing);

        // Checkbox in the middle
        bool uniform_scaling = *state & 1;
        if (ImGui::Checkbox((std::string("##b") + pid).c_str(), &uniform_scaling))
        {
            flag |= 4;
            *state ^= 1; // toggle uniform_scaling
        }

        ImGui::SameLine(0, spacing);

        // Drag Y
        ImGui::PushItemWidth(itemWidth);
        if (ImGui::DragFloat(name, &v[1], 0.0001f, -1.0f, 2.0f, "%f"))
        {
            flag |= 2;
            if (ImGui::IsItemActive() && uniform_scaling)
            {
                if (!(*state & 2))
                {
                    locked_aspect = v[0] / v[1];
                    *state |= 2; // aspect_locked = true
                }
                v[0] = v[1] * (locked_aspect);
            }
        }
        ImGui::PopItemWidth();
        ImGui::PopID();

        // When neither drag is active, release the lock
        if (!ImGui::IsItemActive())
        {
            *state &= 1; // aspected_locked = false
        }
        return flag;
    }
     

    void gui_entity_list()
    {
        adobo::AdoboScene *scene = proj_active_scene();
        ImGui::Begin("Scene Browser");
        
        // ImGui::Text("New Scene:");

        ImGui::Text("Scene: ");
        if (ImGui::BeginCombo(
            "##Scenes", 
            (scene) ? scene->m_name  : "Select...")
        )
        {
            for (i32 n = 0; n < (i32)project.m_scenes.size(); n++)
            {
                bool is_selected = (g_state.active_scene_index == n);
                if (ImGui::Selectable(
                    (std::string(project.m_scenes[n].m_name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(&project.m_scenes[n])))
                    .c_str(), is_selected))
                    g_state.active_scene_index = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // Ensures focus starts here
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete Scene"))
        {
            ImGui::OpenPopup("ConfirmSceneDeletion");
        }
        gui_popup_confirm_delete();

        ImGui::InputText("##scene_name", g_state.buffer_scene, IM_ARRAYSIZE(g_state.buffer_scene));
        ImGui::SameLine();
        if (ImGui::Button("Create Scene")) { proj_create_scene(); }
        

        ImGui::Separator();
       
        ImGui::Text("Entity:");        
        ImGui::InputText("##ent_name", g_state.buffer_ent, IM_ARRAYSIZE(g_state.buffer_ent));
        ImGui::SameLine();
        if (ImGui::Button("Create Entity")) { scene_create_entity(); }

        ImGui::Separator();

        if (scene)
        {
            ImGui::Indent();
            for (i32 i = 0; i < (i32)scene->m_entities.size(); /* manual i++ inside */) {
                // Unique ID per entity
                std::string entlist_id = std::to_string(reinterpret_cast<uintptr_t>(&scene->m_entities[i]));

                bool is_selected = (g_state.sel_ent_index == i &&
                                    g_state.active_scene_index == g_state.sel_ent_scene_index);

                ImGui::PushID(i);

                // Reserve space so the Selectable leaves room for the delete button
                const float btn_w = ImGui::GetFrameHeight(); // square-ish
                const ImGuiStyle& style = ImGui::GetStyle();
                const float full_w = ImGui::GetContentRegionAvail().x;
                const float selectable_w = full_w - (btn_w + style.ItemSpacing.x);

                // Selectable
                if (ImGui::Selectable(
                        (std::string(scene->m_entities[i].m_name) + "##" + entlist_id).c_str(),
                        is_selected,
                        0,
                        ImVec2(selectable_w, 0)))
                {
                    g_state.sel_ent_index = i;
                    g_state.sel_ent_scene_index = g_state.active_scene_index;
                    g_state.sel_hitbox_index = project.m_scenes[g_state.sel_ent_scene_index]
                                                .m_entities[i].m_hitbox_index;
                    g_state.sel_subhitbox_index = -1;
                }

                // Delete button, same line
                ImGui::SameLine();
                if (ImGui::Button(("X##" + entlist_id).c_str(), ImVec2(btn_w, 0))) {
                    // Adjust selection if needed
                    if (g_state.sel_ent_index == i &&
                        g_state.active_scene_index == g_state.sel_ent_scene_index) {
                        g_state.sel_ent_index = -1;
                        g_state.sel_hitbox_index = -1;
                        g_state.sel_subhitbox_index = -1;
                    } else if (g_state.sel_ent_index > i &&
                            g_state.active_scene_index == g_state.sel_ent_scene_index) {
                        g_state.sel_ent_index--;
                    }

                    // Remove entity
                    scene->remove_entity(i);
                    

                    ImGui::PopID();
                    continue; // skip i++ since we erased
                }

                ImGui::PopID();
                i++; // normal advance if not erased
            }
            ImGui::Unindent();
        }

        ImGui::End();
    }

    void gui_entity_inspector()
    {
        adobo::AdoboScene *scene = proj_active_scene();

        ImGui::Begin("Inspector");

        if (
            scene &&
            !scene->m_entities.empty() && 
            is_valid_ent(scene, g_state.sel_ent_index) && 
            g_state.active_scene_index == g_state.sel_ent_scene_index
        )
        {
            i32 &ent_index = g_state.sel_ent_index;
            i32 cur_ent_index = ent_index;
            auto &ent = scene->m_entities[ent_index];
            auto data = ent.m_id();
            auto &atlas = project.m_assets.atlases; 

            ImGui::InputText("Entity##selentityname", ent.m_name, 31);
            
            if (ImGui::InputInt("Layer", &cur_ent_index) && !scene->m_entities.empty())
            {
                if (cur_ent_index >= (i32)scene->m_entities.size()) { cur_ent_index = (i32)scene->m_entities.size() - 1; }
                if (cur_ent_index < 0) { cur_ent_index = 0; }

                std::swap(scene->m_entities[cur_ent_index], scene->m_entities[ent_index]);
                ent_index = cur_ent_index;
            }

            ImGui::InputInt("Type ", &data.type, 1, 100);
            
            {
                ImGui::Text(
                    "Texture: %s - %s" , 
                    (is_valid(ent.m_tex_index)) ? 
                    (project.m_atlas_info[ent.m_tex_index].name) : ("NULL"),
                    (is_valid(ent.m_tex_index, ent.m_subtex_index)) ? 
                    (atlas[ent.m_tex_index].subtex_names[ent.m_subtex_index].c_str()) : ("NULL")
                );
                i32 &counter = ent.m_subtex_index; // persistent value
                const i32 min_val = 0;
                const i32 &max_val = data.tex().sub_n;

                float fullWidth = ImGui::CalcItemWidth(); // same as a normal single widget width
                float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                float halfWidth = (fullWidth - spacing) * 0.5f; // split equally for input + button

                // Button (half)
                ImGui::PushItemWidth(halfWidth); // button auto-sizes, but we can force width
                if (ImGui::Button("Set Texture", ImVec2(halfWidth, 0)) && is_valid(ent.m_tex_index, ent.m_subtex_index))
                {
                    ent.m_tex_index = g_state.sel_tex_index;
                    ent.m_subtex_index = g_state.sel_subtex_index;
                    data.tex = atlas[g_state.sel_tex_index].tex;
                    data.tex_uv = atlas[g_state.sel_tex_index].tex[g_state.sel_subtex_index];
                }
                ImGui::PopItemWidth();
                
                ImGui::SameLine(0, spacing);
                
                // InputInt (half)
                ImGui::PushItemWidth(halfWidth);
                if (ImGui::InputInt("##counterInput", &counter, 1))
                {
                    if (counter < min_val)
                        counter = min_val;
                    if (counter >= max_val)
                        counter = max_val - 1;
                    data.tex_uv = data.tex[counter];
                }
                ImGui::PopItemWidth();

            }

            ImGui::Separator();

            {

                ImGui::DragFloat3("Position", data.position, 0.0001f, -1.0f, 2.0f, "%f");
                ImGui::DragFloat3("Rotation", data.rotation, 0.0001f, glm_rad(-180), glm_rad(180), "%f");

                // Drag X
                static  u8 scale_state = 0;
                component_uniform_dragf2("Scale##entinsscale", data.scale, &scale_state);

                ImGui::Separator();
                gui_section_hitbox_editor();
                /* HITBOX EDITOR */
                if (ImGui::Button("Create hitbox"))
                {
                    proj_create_hitbox();
                }
                ImGui::SameLine();

                if (ImGui::RadioButton("AABB", g_state.hitbox_type == ecs::HITBOX_TYPE_AABB))
                    g_state.hitbox_type = ecs::HITBOX_TYPE_AABB;
                ImGui::SameLine();
                if (ImGui::RadioButton("Circle", g_state.hitbox_type == ecs::HITBOX_TYPE_CIRCLE))
                    g_state.hitbox_type = ecs::HITBOX_TYPE_CIRCLE;

                if (component_input_int("HitboxIndex", &ent.m_hitbox_index, -1, project.m_hitboxes.size()))
                {
                    if (ent.m_hitbox_index < 0)
                    {
                        data.hitbox.id = ecs::g_hitboxes.sparse.invalid_id();
                        g_state.sel_hitbox_index = -1;
                    }
                    else
                    {
                        data.hitbox = project.m_hitboxes[ent.m_hitbox_index];
                        g_state.sel_hitbox_index = ent.m_hitbox_index;
                    }
                }

                if (g_state.sel_hitbox_index < (i32)project.m_hitboxes.size() && g_state.sel_hitbox_index >= 0)
                {
                    i32 &hb_index = g_state.sel_hitbox_index;
                    i32 &rect_index = g_state.sel_subhitbox_index;

                    // AABB EDITOR
                    if (
                        project.m_hitboxes[hb_index].is_valid() && 
                        project.m_hitboxes[hb_index].get_val<ecs::HitboxType>() & ecs::HITBOX_TYPE_AABB)
                    {
                        auto& aabb = project.m_hitboxes[hb_index].get<ecs::HitboxAABB>();
                        
                        if (ImGui::Button("Create SubHitbox"))
                        {
                            rect_index = aabb.size; 
                            aabb.emplace_back(0, 0, 1.0f, 1.0f);
                        }
                        component_input_int("SubIndex", &rect_index, -1, aabb.size);
                        if (aabb.size && rect_index >= 0 && rect_index < aabb.size)
                        {
                            static u8 hb_state = 0;
                            ImGui::DragFloat2("Position##hb", &aabb.data[rect_index].x, 0.0001f, -1.0f, 2.0f, "%f");
                            component_uniform_dragf2("Scale##hbscale", &aabb.data[rect_index].z, &hb_state);
                        }
                    }
                    else
                    {
                        ImGui::Text("No SubHitbox selected");
                    }

                }
            }
        }
        else
        {
            ImGui::Text("No entity selected.");
        }

        ImGui::End();
    }


    void gui_asset_browser()
    {
        static bool is_editing = false;

        ImGui::Begin("Asset Browser");

        if (ImGui::Button("Save File"))    { button_save_proj(); }
        ImGui::SameLine();
        if (ImGui::Button("Load Project")) { button_load_proj(); }
        ImGui::SameLine();
        if (ImGui::Button("Load Atlas"))   { button_load_asset(); }

        ImGui::Separator();

        auto &atlas = project.m_assets.atlases;
        auto &tex_idx = g_state.sel_tex_index;
        auto &subtex_idx = g_state.sel_subtex_index;
        ImGui::Text(
            "Selected Texture: %s (%d) - %s (%d)", 
            (is_valid(tex_idx)) ? 
            (project.m_atlas_info[tex_idx].name) : ("NULL"),
            tex_idx,
            (is_valid(tex_idx, subtex_idx)) ? 
            (atlas[tex_idx].subtex_names[subtex_idx].c_str()) : ("NULL"),
            subtex_idx
        );
        ImGui::Separator();

        for (i32 i = 0; i < (i32)atlas.size(); i++)
        {
            auto &cur_atlas = atlas[i];
            auto &cur_atlas_info = project.m_atlas_info[i];

            bool is_selected = (g_state.sel_tex_index == i);
            if (is_editing && is_selected)
            {
                if (ImGui::InputText("##edit_atlas_name", cur_atlas_info.name, 32,
                                     ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
                {   
                    is_editing = false;
                }
            }
            else
            {
                if (ImGui::Selectable(
                    (std::string(cur_atlas_info.name) + "##" + std::to_string(reinterpret_cast<uintptr_t>(&atlas)))
                    .c_str(), is_selected)) 
                {
                    if (g_state.sel_tex_index == i) { g_state.sel_tex_index = -1; } // deselect
                    else { g_state.sel_tex_index = i; } // select
                    g_state.sel_subtex_index = -1;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    is_editing = true;
                    ImGui::SetKeyboardFocusHere(); // focus text box immediately
                }

                if (is_selected)
                {
                    ImGui::Indent();
                    for (i32 j = 0; j < (i32)cur_atlas.subtex.size(); j++)
                    {
                        auto &cur_subtex = cur_atlas.subtex_names[j];
                        
                        bool is_subselected = (j == g_state.sel_subtex_index);

                        if (ImGui::Selectable(
                            (cur_subtex + "##" + std::to_string(reinterpret_cast<uintptr_t>(&cur_subtex)))
                            .c_str(), is_subselected))
                        {
                            if (is_subselected) { g_state.sel_subtex_index = -1; }// deselect
                            else { g_state.sel_subtex_index = j; }
                        }
                    }
                    ImGui::Unindent();
                }          
            } 
        }

        ImGui::End();

    }

    /**************************************** 
     *                                      *
     * functions                            *
     *                                      *
     ****************************************/

    void file_dialog_gui()
    {
        // LOADING PROJECT
        if (ImGuiFileDialog::Instance()->Display("SingleSelectAdoboProj"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                auto fullPath = ImGuiFileDialog::Instance()->GetFilePathName();
                auto name = ImGuiFileDialog::Instance()->GetCurrentFileName();

                adobo::release_adoboproj(project);
                adobo::read_adoboproj(project, fullPath.c_str());
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // LOADING ATLAS
        if (ImGuiFileDialog::Instance()->Display("MultiSelectAtlas"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                auto selections = ImGuiFileDialog::Instance()->GetSelection();
                static int i = 0;
                for (const auto &sel : selections)
                {
                    std::string name =     sel.first;      // just filename
                    std::string fullPath = sel.second; // full path (dir + filename)
                    
                    project.load_atlas(fullPath.c_str(), (name + "_" + std::to_string(i)).c_str());
                    i++;
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        // SAVING PROJ
        if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                // DEBUG_LOG("WRITING OUT PROJECT\n");
                adobo::write_adoboproj(project, filePathName.c_str());
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

    void button_save_proj()
    {
        IGFD::FileDialogConfig config;
        config.path = ".";                                    // Default directory
        config.filePathName = "untitled.adoboproj";           // Pre-filled filename
        config.countSelectionMax = 1;                         // Only 1 file
        config.flags = ImGuiFileDialogFlags_ConfirmOverwrite; // Save mode

        ImGuiFileDialog::Instance()->OpenDialog(
            "SaveFileDlgKey", // Key
            "Save File",      // Title
            ".adoboproj",     // Filters
            config);
    }

    void button_load_proj()
    {
        IGFD::FileDialogConfig cfg;
        cfg.path = ".";            // Initial folder
        cfg.countSelectionMax = 1; // Only one file allowed
        cfg.flags = ImGuiFileDialogFlags_None;

        ImGuiFileDialog::Instance()->OpenDialog(
            "SingleSelectAdoboProj",  // Dialog key
            "Choose adoboproj files", // Window title
            ".adoboproj",                     // File filter
            cfg                       // Config struct
        );
    }

    void button_load_asset()
    {
        IGFD::FileDialogConfig cfg;
        cfg.path = ".";
        cfg.countSelectionMax = 0; // 0 means unlimited multi-select
        cfg.flags = ImGuiFileDialogFlags_None;

        ImGuiFileDialog::Instance()->OpenDialog(
            "MultiSelectAtlas",
            "Choose image or atlas Files",
            ".xml,.png,.jpeg,.bmp,.*",
            cfg);
    }

    bool is_valid(const i32 tex_index)
    {
        return tex_index >= 0 &&  tex_index < (i32)project.m_assets.atlases.size();   
    }    

    bool is_valid(const i32 tex_index,const i32 subtex_index)
    {
        return 
            is_valid(tex_index) && 
            subtex_index >= 0 && 
            subtex_index < (i32)project.m_assets.atlases[tex_index].subtex.size();
    }

    bool is_valid_ent(adobo::AdoboScene *scene, i32 ent_index)
    {
        if (scene)
        {
            return ent_index >= 0 && ent_index < (i32)scene->m_entities.size();
        }
        return false;
    }

    bool is_valid_ent(i32 ent_index)
    {
        adobo::AdoboScene *scene = nullptr;
        if ((scene = proj_active_scene()))
        {
            return is_valid_ent(scene, ent_index);
        }  
        return false; 
    }

    adobo::AdoboScene *proj_active_scene()
    {
        if (g_state.active_scene_index >= 0 && g_state.active_scene_index >= (i32)project.m_scenes.size())
        {
            // DEBUG_ERR("NO VALID SCENES\n");
            return nullptr;
        }
        return &project.m_scenes[g_state.active_scene_index];
    }
}