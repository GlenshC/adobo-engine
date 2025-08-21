#include "game/editor.h"
#include <vector>
#include "imgui.h"
#include "core/entity/ecs.h"
#include "types.h"


namespace editor
{
    struct Scene
    {
    };

    void create_entity(const char *e_name)
    {

    }

    void editor_gui()
    {
        static ecs::Entity2D *selected_entity = nullptr;
        static char new_entity_name[64] = "New Entity";

        // === Scene Browser ===
        ImGui::Begin("Scene Browser");

        if (ImGui::Button("Create New Entity"))
        {
            scene.CreateEntity(new_entity_name);

            ecs::Entity2D new_entity;
            ecs::create(new_entity); 
            selected_entity = &new_entity;
        }

        ImGui::Separator();

        for (auto &entity : scene.GetEntities())
        {
            bool isSelected = (selectedEntity == &entity);
            if (ImGui::Selectable(entity.GetName().c_str(), isSelected))
            {
                selectedEntity = &entity;
            }
        }

        ImGui::End();

        // === Inspector ===
        ImGui::Begin("Inspector");

        if (selectedEntity)
        {
            ImGui::Text("Entity: %s", selectedEntity->GetName().c_str());
            Transform &transform = selectedEntity->GetComponent<Transform>();
            ImGui::InputFloat3("Position", transform.position);
            ImGui::InputFloat3("Rotation", transform.rotation);
            ImGui::InputFloat3("Scale", transform.scale);
            // Add more component editors here
        }
        else
        {
            ImGui::Text("No entity selected.");
        }

        ImGui::End();

        // === Asset Browser ===
        ImGui::Begin("Asset Browser");

        for (const auto &asset : assetList)
        {
            ImGui::Selectable(asset.name.c_str());
            // Optionally show thumbnails or drag-and-drop
        }

        ImGui::End();
    }
}