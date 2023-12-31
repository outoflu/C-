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
        assert(image.height > 0 && image.width > 0);
        this->height = image.height;
        this->width = image.width;
        this->buffer = new vec4_t[width * height];
        if (image.format == FORMAT_LDR)
        {
            ldr_image_to_texture(image);
            if (usage == USAGE_HDR_COLOR)
            {
                srgb_to_linear();
            }
        }
        else
        {
            hdr_image_to_texture(image);
            if (usage == USAGE_LDR_COLOR)
            {
                linear_to_srgb();
            }
        }
    }

    void texture_t::set_width(int _width)
    {
        this->width = _width;
    }

    void texture_t::set_height(int _height)
    {
        this->height = _height;
    }

    int texture_t::get_width()
    {
        return this->width;
    }

    int texture_t::get_height()
    {
        return this->height;
    }

    texture_t::~texture_t()
    {
        delete[] buffer;
    }

    void texture_t::texture_from_colorbuffer(framebuffer_t &framebuffer)
    {
        int num_pixels = width * height;

        assert(width == framebuffer.get_width());
        assert(height == framebuffer.get_height());

        for (int i = 0; i < num_pixels; ++i)
        {
            auto color = &framebuffer.color_buffer[i * 4];
            float r = float_from_uchar(color[0]);
            float g = float_from_uchar(color[1]);
            float b = float_from_uchar(color[2]);
            float a = float_from_uchar(color[3]);
            buffer[i] = vec4_new(r, g, b, a);
        }
    }
    void texture_t::texture_from_depthbuffer(framebuffer_t &framebuffer)
    {
        int num_pixels = width * height;
        int i;

        assert(width == framebuffer.get_width());
        assert(height == framebuffer.get_height());

        for (i = 0; i < num_pixels; i++)
        {
            float depth = framebuffer.depth_buffer[i];
            buffer[i] = vec4_new(depth, depth, depth, 1);
        }
    }
    vec4_t texture_t::texture_repeat_sample(const vec2_t &texcoord)
    {
        float u = texcoord.x - (float)floor(texcoord.x);
        float v = texcoord.y - (float)floor(texcoord.y);

        int c = (int)((width - 1) * u);
        int r = (int)((height - 1) * v);
        int index = r * width + c;
        return buffer[index];
    }
    vec4_t texture_t::texture_clamp_sample(const vec2_t &texcoord)
    {
        float u = float_saturate(texcoord.x);
        float v = float_saturate(texcoord.y);
        int c = (int)((width - 1) * u);
        int r = (int)((height - 1) * v);
        int index = r * width + c;
        return buffer[index];
    }
    vec4_t texture_t::texture_sample(const vec2_t &texcoord)
    {
        return texture_repeat_sample(texcoord);
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

    cubemap_t::cubemap_t(const char *positive_x, const char *negative_x, const char *positive_y, const char *negative_y, const char *positive_z, const char *negative_z, usage_t usage)
    {
        faces[0] = new texture_t(positive_x, usage);
        faces[1] = new texture_t(negative_x, usage);
        faces[2] = new texture_t(positive_y, usage);
        faces[3] = new texture_t(negative_y, usage);
        faces[4] = new texture_t(positive_z, usage);
        faces[5] = new texture_t(negative_z, usage);
    }

    cubemap_t::~cubemap_t()
    {
        for (int i = 0; i < 6; i++)
        {
            delete faces[i];
        }
    }
    static int select_cubemap_face(const vec3_t &direction, vec2_t &texcoord)
    {
        float abs_x = (float)fabs(direction.x);
        float abs_y = (float)fabs(direction.y);
        float abs_z = (float)fabs(direction.z);
        float ma, sc, tc;
        int face_index;
        if (abs_x > abs_y && abs_x > abs_z)
        { /* major axis -> x */
            ma = abs_x;
            if (direction.x > 0)
            { /* positive x */
                face_index = 0;
                sc = -direction.z;
                tc = -direction.y;
            }
            else
            { /* negative x */
                face_index = 1;
                sc = +direction.z;
                tc = -direction.y;
            }
        }
        else if (abs_y > abs_z)
        { /* major axis -> y */
            ma = abs_y;
            if (direction.y > 0)
            { /* positive y */
                face_index = 2;
                sc = +direction.x;
                tc = +direction.z;
            }
            else
            { /* negative y */
                face_index = 3;
                sc = +direction.x;
                tc = -direction.z;
            }
        }
        else
        { /* major axis -> z */
            ma = abs_z;
            if (direction.z > 0)
            { /* positive z */
                face_index = 4;
                sc = +direction.x;
                tc = -direction.y;
            }
            else
            { /* negative z */
                face_index = 5;
                sc = -direction.x;
                tc = -direction.y;
            }
        }

        texcoord.x = (sc / ma + 1) / 2;
        texcoord.y = (tc / ma + 1) / 2;
        return face_index;
    }
    vec4_t cubemap_t::cubemap_repeat_sample(const vec3_t &direction)
    {
        vec2_t texcoord;
        int face_index=select_cubemap_face(direction,texcoord);
        texcoord.y=1-texcoord.y;
        return faces[face_index]->texture_repeat_sample(texcoord);
    }
    vec4_t cubemap_t::cubemap_clamp_sample(const vec3_t &direction)
    {
         vec2_t texcoord;
        int face_index=select_cubemap_face(direction,texcoord);
        texcoord.y=1-texcoord.y;
        return faces[face_index]->texture_clamp_sample(texcoord);
    }
    vec4_t cubemap_t::cubemap_sample(const vec3_t &direction)
    {
        return cubemap_repeat_sample(direction);
    }
}