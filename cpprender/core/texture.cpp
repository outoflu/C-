#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "graphics.hpp"
#include "image.hpp"
#include "maths.hpp"
#include "texture.hpp"

namespace core
{
    texture_t::texture_t(int _width, int _height) : width(_width), height(_height)
    {
        assert(width > 0 && height > 0);
        this->buffer = new vec4_t[width * height];
        memset(buffer, 0, sizeof(vec4_t) * height * width);
    }
    texture_t::texture_t(const char *filename, usage_t usage)
    {
    }

    texture_t::~texture_t()
    {
        delete[] buffer;
    }

    void texture_t::texture_from_colorbuffer(framebuffer_t &framebuffer)
    {
    }
    void texture_t::texture_from_depthbuffer(framebuffer_t &framebuffer)
    {
    }
    vec4_t texture_t::texture_repeat_sample(const vec2_t &texcoord)
    {
        return vec4_t();
    }
    vec4_t texture_t::texture_clamp_sample(const vec2_t &texcoord)
    {
        return vec4_t();
    }
    vec4_t texture_t::texture_sample(const vec2_t &texcoord)
    {
        return vec4_t();
    }
    void texture_t::ldr_image_to_texture(const image_t &image)
    {
    }
    void texture_t::hdr_image_to_texture(const image_t &image)
    {
    }
    void texture_t::srgb_to_linear()
    {
        int num_pixels = width * height;
        int i;

        for (i = 0; i < num_pixels; i++)
        {
            vec4_t *pixel = &buffer[i];
            pixel->x = float_srgb2linear(pixel->x);
            pixel->y = float_srgb2linear(pixel->y);
            pixel->z = float_srgb2linear(pixel->z);
        }
    }
    void texture_t::linear_to_srgb()
    {
        int num_pixels = width * height;
        int i;

        for (i = 0; i < num_pixels; i++)
        {
            vec4_t *pixel = &buffer[i];
            pixel->x = float_linear2srgb(float_aces(pixel->x));
            pixel->y = float_linear2srgb(float_aces(pixel->y));
            pixel->z = float_linear2srgb(float_aces(pixel->z));
        }
    }
}