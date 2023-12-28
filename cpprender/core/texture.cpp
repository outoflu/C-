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
        image_t image;
        image.image_load(filename);
        assert(image.height>0&&image.width>0);
        this->height=image.height;
        this->width=image.width;
        this->buffer=new vec4_t[width*height];
        if (image.format == FORMAT_LDR){
            ldr_image_to_texture(image);
            if (usage==USAGE_HDR_COLOR){
                srgb_to_linear();
            }
        } else {
            hdr_image_to_texture(image);
            if (usage==USAGE_LDR_COLOR){
                linear_to_srgb();
            }
        }
        
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
        int num_pixels = image.width * image.height;
        int i;

        for (i = 0; i < num_pixels; i++)
        {
            unsigned char *pixel = &image.ldr_buffer[i * image.channels];
            vec4_t texel = {0, 0, 0, 1};
            if (image.channels == 1)
            { /* GL_LUMINANCE */
                texel.x = texel.y = texel.z = float_from_uchar(pixel[0]);
            }
            else if (image.channels == 2)
            { /* GL_LUMINANCE_ALPHA */
                texel.x = texel.y = texel.z = float_from_uchar(pixel[0]);
                texel.w = float_from_uchar(pixel[1]);
            }
            else if (image.channels == 3)
            { /* GL_RGB */
                texel.x = float_from_uchar(pixel[0]);
                texel.y = float_from_uchar(pixel[1]);
                texel.z = float_from_uchar(pixel[2]);
            }
            else
            { /* GL_RGBA */
                texel.x = float_from_uchar(pixel[0]);
                texel.y = float_from_uchar(pixel[1]);
                texel.z = float_from_uchar(pixel[2]);
                texel.w = float_from_uchar(pixel[3]);
            }
            buffer[i] = texel;
        }
    }
    void texture_t::hdr_image_to_texture(const image_t &image)
    {
        int num_pixels = image.width * image.height;
        int i;

        for (i = 0; i < num_pixels; i++)
        {
            float *pixel = &image.hdr_buffer[i * image.channels];
            vec4_t texel = {0, 0, 0, 1};
            if (image.channels == 1)
            { /* GL_LUMINANCE */
                texel.x = texel.y = texel.z = pixel[0];
            }
            else if (image.channels == 2)
            { /* GL_LUMINANCE_ALPHA */
                texel.x = texel.y = texel.z = pixel[0];
                texel.w = pixel[1];
            }
            else if (image.channels == 3)
            { /* GL_RGB */
                texel.x = pixel[0];
                texel.y = pixel[1];
                texel.z = pixel[2];
            }
            else
            { /* GL_RGBA */
                texel.x = pixel[0];
                texel.y = pixel[1];
                texel.z = pixel[2];
                texel.w = pixel[3];
            }
            buffer[i] = texel;
        }
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