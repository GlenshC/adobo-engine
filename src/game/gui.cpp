#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "util/debug_state.h"
#include "util/debug.h"
#include "core/platform.h"
#include "core/clock.h"
#include "core/gui.h"
#include "core/game.h"

#include <cmath>

namespace gui
{
    static ImGuiIO *io;
    static void RenderDebugOverlay(void);

    void display_gui(void) {
    }
    
    void init(void) {
        // Init ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO();
        io->IniFilename = nullptr;
        io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io->FontGlobalScale = 1.7f;
        // io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
        ImGui::StyleColorsDark();
    
        // Setup ImGui Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(plat::g_window.handle, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void render(void)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // display_gui();
        // RenderDebugOverlay();
        game::gui();
        // ImGui::ShowDemoWindow(); // Optional
    
        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    void shutdown(void)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    __attribute__((unused)) static void RenderDebugOverlay(void)
    {
        static f32 time_e = 0;
        static f32 fps_imgui = 0;
        static i32 fps_game = 0;
        
        time_e += clk::g_time.delta;
        if (time_e >= 1.0)
        {
            fps_imgui = io->Framerate;
            fps_game = clk::g_fps.fps;
            time_e = 0;
        }

        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        // Position at top-left
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);

        // Transparent background
        ImGui::SetNextWindowBgAlpha(0.35f); // 0.0 = fully transparent, 1.0 = solid

        if (ImGui::Begin("Debug Overlay", nullptr, window_flags))
        {
            ImGui::Text("Game  FPS: %d", fps_game);
            ImGui::Text("ImGui FPS: %.1f", fps_imgui);
        }
        ImGui::End();
    }
}