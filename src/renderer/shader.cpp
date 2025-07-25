#include <cstdio>
#include <glad/gl.h>

#include "shader.h"
#include "util/string.h"
#include "util/logger.h"

namespace shader
{
    static u32  _shader_link(u32 vert_id, u32 frag_id);
    static u32  _shader_compile(const char *source, u32 type);

    void create(Shader &shader, const char *vertPath, const char *fragPath)
    {
        char *vertSource = util::string_readf(vertPath);
        char *fragSource = util::string_readf(fragPath);

        u32 vert_id = _shader_compile(vertSource, GL_VERTEX_SHADER);
        u32 frag_id = _shader_compile(fragSource, GL_FRAGMENT_SHADER);
        if (vert_id == 0 || frag_id == 0)
        {
            glDeleteShader(vert_id);
            glDeleteShader(frag_id);
            return;
        }
        shader.vertPath = vertPath;
        shader.fragPath = fragPath;
        shader.id = _shader_link(vert_id, frag_id);
    }

    void bind(Shader &shader)
    {
        glUseProgram(shader.id);
    }

    void unbind(void)
    {
        glUseProgram(0);
    }

    void set_uniform_mat4(const Shader &shader, const char *name, mat4 value)
    {
        glUniformMatrix4fv(
            glGetUniformLocation(shader.id, name), 1,
            GL_FALSE, (float *) value
        );
    }

    void set_uniform_vec4(const Shader &shader, const char *name, vec4 value)
    {
        glUniform4fv(
            glGetUniformLocation(shader.id, name), 
            1, (float *) value
        );
    }

    void set_uniform_vec2(const Shader &shader, const char *name, vec2 value)
    {
        glUniform2fv(
            glGetUniformLocation(shader.id, name), 
            1, (float *) value
        );
    }

    void set_uniform_f32(const Shader &shader, const char *name, f32 value)
    {
        glUniform1f(
            glGetUniformLocation(shader.id, name), 
            value
        );
    }

    void set_uniform_i32(const Shader &shader, const char *name, i32 value)
    {
        glUniform1i(
            glGetUniformLocation(shader.id, name), 
            value
        );
    }

    // overload for shader_id
    void set_uniform_mat4(const u32 shader_id, const char *name, mat4 value)
    {
        glUniformMatrix4fv(
            glGetUniformLocation(shader_id, name), 1,
            GL_FALSE, (float *) value
        );
    }

    void set_uniform_vec4(const u32 shader_id, const char *name, vec4 value)
    {
        glUniform4fv(
            glGetUniformLocation(shader_id, name), 
            1, (float *) value
        );
    }

    void set_uniform_vec2(const u32 shader_id, const char *name, vec2 value)
    {
        glUniform2fv(
            glGetUniformLocation(shader_id, name), 
            1, (float *) value
        );
    }

    void set_uniform_f32(const u32 shader_id, const char *name, f32 value)
    {
        glUniform1f(
            glGetUniformLocation(shader_id, name), 
            value
        );
    }

    void set_uniform_i32(const u32 shader_id, const char *name, i32 value)
    {
        glUniform1i(
            glGetUniformLocation(shader_id, name), 
            value
        );
    }

    static u32  _shader_link(u32 vert_id, u32 frag_id)
    {
        u32 shader_id = glCreateProgram();
        glAttachShader(shader_id, vert_id);
        glAttachShader(shader_id, frag_id);
        glLinkProgram(shader_id);

        glDeleteShader(vert_id);
        glDeleteShader(frag_id);

        i32 status;
        glGetProgramiv(shader_id, GL_LINK_STATUS, &status);
        if (!status)
        {
            i32 info_len = 0;
            char info_log[1024];
            glGetProgramInfoLog(shader_id, 1024, &info_len, info_log);
            DEBUG_ERR("Shader linking error: %s\n", info_log);
            return 0;
        }
        DEBUG_LOG("Shader linked.\n");
        return shader_id;
    }

    static u32  _shader_compile(const char *source, u32 type)
    {
        unsigned int shader_id = glCreateShader(type);
        glShaderSource(shader_id, 1, &source, NULL);
        glCompileShader(shader_id);

        int status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            int info_len = 0;
            char info_log[1024];
            glGetShaderInfoLog(shader_id, 1024, &info_len, info_log);
            DEBUG_ERR("Shader compilation error: %s\n", info_log);
            return 0;
        }
        DEBUG_LOG("Shader compiled.\n");
        return shader_id;
    }

}