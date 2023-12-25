#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "maths.h"
#include "graphics.h"

namespace core
{

    framebuffer_t::framebuffer_t(int _width, int _height) : width(_width), height(_height)
    {
        vec4_t default_color = {0, 0, 0, 1};
        float default_depth = 1.0;
        assert(_width > 0 && _height > 0);
        auto buffer_size = _width * _height;
        this->color_buffer = std::make_unique<unsigned char[]>(buffer_size * 4);
        this->depth_buffer = std::make_unique<float[]>(buffer_size);

        clear_color(default_color);
        clear_depth(default_depth);
    }

    void framebuffer_t::set_width(int _width)
    {
        this->width = _width;
    }
    int framebuffer_t::get_width()
    {
        return width;
    }

    void framebuffer_t::set_height(int _height)
    {
        this->height = _height;
    }
    int framebuffer_t::get_height()
    {
        return height;
    }

    framebuffer_t::~framebuffer_t()
    {
    }

    void framebuffer_t::clear_color(vec4_t color)
    {
        int num_pixels = width * height;
        for (int i = 0; i < num_pixels; ++i)
        {
            color_buffer[i * 4 + 0] = float_to_uchar(color.x);
            color_buffer[i * 4 + 1] = float_to_uchar(color.y);
            color_buffer[i * 4 + 2] = float_to_uchar(color.z);
            color_buffer[i * 4 + 3] = float_to_uchar(color.w);
        }
    }

    void framebuffer_t::clear_depth(float depth)
    {
        int num_pixels = width * height;
        for (int i = 0; i < num_pixels; ++i)
        {
            depth_buffer[i] = depth;
        }
    }

    template<typename _Attribs, typename _Varyings, typename _Uniforms>
    program_t<_Attribs, _Varyings, _Uniforms>::program_t(vertex_shader_t, fragment_shader_t, int double_sided, int enable_blend)
    {
        
    }

} // core