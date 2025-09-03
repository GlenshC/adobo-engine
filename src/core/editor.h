#pragma once

#include "core/adoboproj.h"
namespace editor
{
    void editor_gui();
    void editor_render();
    void editor_init();
    void editor_save();
    adobo::AdoboScene *proj_active_scene();
}