/*
    //header
    "adoboproj"

    i32 num_atlas
    i32 num_shader
    i32 num_scenes
    i32 num_hitboxaabb
    i32 num_hitboxcircle
    i32 num_total_entities
    i32 num_total_path_size // atlas and shdaer path size only

    // metadata
    i32[] n_entities;
    i32[] atlas_file_path_size
    i32[] shader_file_path_size
    i32[] n_subhbaabb;
    i32[] n_subhbcircle;
    
    // data
    char[32] project_name

    // per scene
    [] char[32] scene_name
    
    [] {
        char[32] name;
        vec3f    position;
        vec2f    scale;
        vec3f    rotation;
        i32      tex_index;
        i32      subtex_index;
        i32      type;
    }

    // TODO IMPLEMENT
    [] hitboxesAABB {
        vec4f[] subhitbox
    }
    [] hitboxCircle{
        vec3f[] subhitbox
    }

    [] char[32] atlas_name
    [] char[32] shader_name
    str[] atlas_file_path
    str[] shader_file_path

*/