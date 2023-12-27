#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "graphics.hpp"
#include "maths.hpp"
#include <array>

namespace core{

    enum usage_t{
        USAGE_LDR_COLOR,
        USAGE_LDR_DATA,
        USAGE_HDR_COLOR,
        USAGE_HDR_DATA
    };

    class texture_t{
    public:
        int width,height;
        vec4_t* buffer;
        texture_t(int _width,int _height);
        texture_t(const char* filename,usage_t usage);
        ~texture_t();
        void texture_from_colorbuffer(framebuffer_t& framebuffer);
        void texture_from_depthbuffer(framebuffer_t& framebuffer);
        vec4_t texture_repeat_sample(const vec2_t& texcoord);
        vec4_t texture_clamp_sample(const vec2_t& texcoord);
        vec4_t texture_sample(const vec2_t& texcoord);
    private:
        void ldr_image_to_texture(const image_t& image);
        void hdr_image_to_texture(const image_t& image);
        void srgb_to_linear();
        void linear_to_srgb();
    };

    struct cubemap_t{
        std::array<texture_t*,6> faces;
        cubemap_t(const char *positive_x, const char *negative_x,
                  const char *positive_y, const char *negative_y,
                  const char *positive_z, const char *negative_z,
                  usage_t usage);
        ~cubemap_t();
        vec4_t cubemap_repeat_sample(const vec3_t& direction);
        vec4_t cubemap_clamp_sample(const vec3_t& direction);
        vec4_t cubemap_sample(const vec3_t& direction);
                
    };

} // core

#endif // _TEXTURE_H