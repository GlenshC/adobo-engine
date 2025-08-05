#pragma once

#include "types.h"
#include "renderer/r_2D.h"
namespace ui
{
    struct UIElement
    {
        struct UIElement *parent;
        struct UIElement **children;
        i32 count_child;

        adobo::vec2f position;
        adobo::vec2f size;

        /*
        bool visible;
        bool enabled;
        */
        
    };

    /*
        begin 
        
        element_func
        
        end
    */
    
}