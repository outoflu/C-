#ifndef _PRIVATE_H
#define _PRIVATE_H

#include "graphics.hpp"
#include "image.hpp"

namespace core{
    void private_blit_bgr(framebuffer_t& source,image_t& target);
    void private_blit_rgb(framebuffer_t& source,image_t& target);

    const char* private_get_extension(const char* filename);
}


#endif //_PRIVATE_H