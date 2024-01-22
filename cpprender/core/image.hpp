#ifndef _IMAGE_H
#define _IMAGE_H

#include <memory>

namespace core
{

    enum format_t
    {
        FORMAT_LDR,
        FORMAT_HDR
    };

    class image_t
    {
    public:
        format_t format;
        int width, height, channels;

        unsigned char *ldr_buffer;
        float *hdr_buffer;
        image_t();
        image_t(const image_t &) = delete;
        image_t operator=(const image_t &) = delete;
        image_t(int width, int height, int channels, format_t format);
        void image_create(int width, int height, int channels, format_t format);
        void image_load(const char *filename);
        void load_tga_rle_payload(FILE* file);
        void load_tga_image(const char* filename);
        void read_hdr_flat_scanline(FILE* file,int row);
        void read_hdr_rle_scanline(FILE* file,int row);
        void read_hdr_scanline(FILE* file,int row);
        void load_hdr_image(const char* filename);
        void image_save(const char *filename);
        void save_tga_image(const char* filename);
        void save_hdr_image(const char* filename);
        void image_flip_h();
        void image_flip_v();
        unsigned char* get_ldr_pixel(int row,int col);
        float* get_hdr_pixel(int row,int col);
        int get_num_elems();
        ~image_t();
    };

    /* image creating/releasing */
    // image_t *image_create(int width, int height, int channels, format_t format);
    // void image_release(image_t *image);
    // image_t *image_load(const char *filename);
    // void image_save(image_t *image, const char *filename);
    /* image processing */
    // void image_flip_h(image_t *image);
    // void image_flip_v(image_t *image);

} // core
#endif //_IMAGE_H
