#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "image.hpp"
#include "macro.hpp"
#include "maths.hpp"
#include "private.hpp"

namespace core
{
    image_t::image_t():format(FORMAT_LDR),width(0),height(0),channels(0),ldr_buffer(nullptr),hdr_buffer(nullptr)
    {
    }
    image_t::image_t(int _width, int _height, int _channels, format_t _format)
    {
        image_create(_width,_height,_channels,_format);
    }
    void image_t::image_create(int _width, int _height, int _channels, format_t _format)
    {
        int num_elems=_width*_height*_channels;
        assert(_width>0&&_height>0&&_channels>=1&&_channels<=4);
        assert(format==FORMAT_LDR||format==FORMAT_HDR);

        this->format=_format;
        this->width=_width;
        this->height=_height;
        this->channels=_channels;
        this->ldr_buffer=nullptr;
        this->hdr_buffer=nullptr;

        if (format==FORMAT_LDR){
            this->ldr_buffer=new unsigned char[num_elems];
            memset(ldr_buffer,0,sizeof(unsigned char)*num_elems);
        } else {
            this->hdr_buffer=new float[num_elems];
            memset(hdr_buffer,0,sizeof(float)*num_elems);
        }
    }
    void image_t::image_load(const char *filename)
    {
    }
    void image_t::load_tga_image(const char *filename)
    {
    }
    void image_t::load_hdr_image(const char *filename)
    {
    }
    void image_t::image_save(const char *filename)
    {
    }
    void image_t::save_tga_image(const char *filename)
    {
    }
    void image_t::save_hdr_image(const char *filename)
    {
    }
    void image_t::image_flip_h()
    {
    }
    void image_t::image_flip_v()
    {
    }
    unsigned char *image_t::get_ldr_pixel(int row, int col)
    {
        return nullptr;
    }
    float *image_t::get_hdr_pixel(int row, int col)
    {
        return nullptr;
    }
    int image_t::get_num_elems()
    {
        return 0;
    }
    image_t::~image_t()
    {
        if (ldr_buffer!=nullptr)
            delete[] ldr_buffer;
        if (hdr_buffer!=nullptr)
            delete[] hdr_buffer;
    }
} // namespace core
