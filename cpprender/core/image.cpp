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
    void swap_bytes(unsigned char *a, unsigned char *b)
    {
        unsigned char t = *a;
        *a = *b;
        *b = t;
    }
    void swap_floats(float *a, float *b)
    {
        float t = *a;
        *a = *b;
        *b = t;
    }
#define TGA_HEADER_SIZE 18
    unsigned char read_byte(FILE *file)
    {
        int byte = fgetc(file);
        assert(byte != EOF);
        return (unsigned char)byte;
    }
    void read_bytes(FILE *file, void *buffer, int size)
    {
        int count = (int)fread(buffer, 1, size, file);
        assert(count == size);
    }
    void write_bytes(FILE *file, void *buffer, int size)
    {
        int count = (int)fwrite(buffer, 1, size, file);
        assert(count == size);
    }

    void read_line(FILE *file, char line[LINE_SIZE])
    {
        if (fgets(line, LINE_SIZE, file) == NULL)
        {
            assert(0);
        }
    }

    int starts_with(const char *string, const char *prefix)
    {
        return strncmp(string, prefix, strlen(prefix)) == 0;
    }

    void rgbe_to_floats(unsigned char rgbe[4], float floats[3])
    {
        float rm = rgbe[0]; /* red mantissa */
        float gm = rgbe[1]; /* green mantissa */
        float bm = rgbe[2]; /* blue mantissa */
        int eb = rgbe[3];   /* exponent biased */
        if (eb == 0)
        {
            floats[0] = floats[1] = floats[2] = 0;
        }
        else
        {
            int ev = eb - 128; /* exponent value */
            float factor = (float)((1.0 / 256) * pow(2, ev));
            floats[0] = rm * factor; /* red value */
            floats[1] = gm * factor; /* green value */
            floats[2] = bm * factor; /* blue value */
        }
    }

    void floats_to_rgbe(float floats[3], unsigned char rgbe[4])
    {
        float rv = floats[0]; /* red value */
        float gv = floats[1]; /* green value */
        float bv = floats[2]; /* blue value */
        float max_v = float_max(float_max(rv, gv), bv);
        if (max_v < 1e-32f)
        {
            rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
        }
        else
        {
            int ev; /* exponent value */
            float max_m = (float)frexp(max_v, &ev);
            float factor = (1 / max_v) * max_m * 256;
            rgbe[0] = (unsigned char)(rv * factor); /* red mantissa */
            rgbe[1] = (unsigned char)(gv * factor); /* green mantissa */
            rgbe[2] = (unsigned char)(bv * factor); /* blue mantissa */
            rgbe[3] = (unsigned char)(ev + 128);    /* exponent biased */
        }
    }
    image_t::image_t() : format(FORMAT_LDR), width(0), height(0), channels(0), ldr_buffer(nullptr), hdr_buffer(nullptr)
    {
    }
    image_t::image_t(int _width, int _height, int _channels, format_t _format)
    {
        image_create(_width, _height, _channels, _format);
    }
    void image_t::image_create(int _width, int _height, int _channels, format_t _format)
    {
        int num_elems = _width * _height * _channels;
        assert(_width > 0 && _height > 0 && _channels >= 1 && _channels <= 4);
        assert(format == FORMAT_LDR || format == FORMAT_HDR);

        this->format = _format;
        this->width = _width;
        this->height = _height;
        this->channels = _channels;
        this->ldr_buffer = nullptr;
        this->hdr_buffer = nullptr;

        if (format == FORMAT_LDR)
        {
            this->ldr_buffer = new unsigned char[num_elems];
            memset(ldr_buffer, 0, sizeof(unsigned char) * num_elems);
        }
        else
        {
            this->hdr_buffer = new float[num_elems];
            memset(hdr_buffer, 0, sizeof(float) * num_elems);
        }
    }

    void image_t::image_load(const char *filename)
    {
    }

    void image_t::load_tga_rle_payload(FILE *file)
    {
        int num_elems = this->get_num_elems();
        int curr_size = 0;
        while (curr_size < num_elems)
        {
            unsigned char header = read_byte(file);
            int rle_packet = header & 0x80;
            int num_pixels = (header & 0x7F) + 1;
            unsigned char pixel[4];
            int i, j;
            assert(curr_size + num_pixels * channels <= num_elems);
            if (rle_packet)
            { /* rle packet */
                for (j = 0; j < channels; j++)
                {
                    pixel[j] = read_byte(file);
                }
                for (i = 0; i < num_pixels; i++)
                {
                    for (j = 0; j < channels; j++)
                    {
                        ldr_buffer[curr_size++] = pixel[j];
                    }
                }
            }
            else
            { /* raw packet */
                for (i = 0; i < num_pixels; i++)
                {
                    for (j = 0; j < channels; j++)
                    {
                        ldr_buffer[curr_size++] = read_byte(file);
                    }
                }
            }
        }
        assert(curr_size == num_elems);
    }

    static void read_tga_header(FILE *file, int &width, int &height, int &channels, int &is_rle, int &flip_h, int &flip_v)
    {
        unsigned char header[TGA_HEADER_SIZE];
        int depth, idlength, imgtype, imgdesc;

        read_bytes(file, header, TGA_HEADER_SIZE);
        width = header[12] | (header[13] << 8);
        height = header[14] | (header[15] << 8);
        assert(width > 0 && height > 0);

        depth = header[16];
        assert(depth == 8 || depth == 24 || depth == 32);
        channels = depth / 8;

        idlength = header[0];
        assert(idlength == 0);

        imgtype = header[2];
        assert(imgtype == 2 || imgtype == 3 || imgtype == 10 || imgtype == 11);
        is_rle = imgtype == 10 || imgtype == 11;

        imgdesc = header[17];
        flip_h = imgdesc & 0x10;
        flip_v = imgdesc & 0x20;
    }

    void image_t::load_tga_image(const char *filename)
    {
        FILE *file = fopen(filename, "rb");
        assert(file != NULL);
        int is_rle, flip_h, flip_v;
        read_tga_header(file, width, height, channels, is_rle, flip_h, flip_v);
        format = FORMAT_LDR;
        if (is_rle)
        {
            load_tga_rle_payload(file);
        }
        else
        {
            read_bytes(file, ldr_buffer, get_num_elems());
        }
        fclose(file);

        if (flip_h)
        {
            image_flip_h();
        }

        if (flip_v)
        {
            image_flip_v();
        }

        if (channels >= 3)
        {
            int r, c;
            for (r = 0; r < height; r++)
            {
                for (c = 0; c < width; c++)
                {
                    unsigned char *pixel = get_ldr_pixel(r, c);
                    swap_bytes(&pixel[0], &pixel[2]);
                }
            }
        }
    }

    void read_hdr_header(FILE *file, int &width, int &height)
    {
        char line[LINE_SIZE];
        int header_found = 0;
        int format_found = 0;
        int items;

        read_line(file, line);
        assert(starts_with(line, "#?"));

        while (1)
        {
            read_line(file, line);
            if (strlen(line) == 1 && line[0] == '\n')
            {
                header_found = 1;
                break;
            }
            else if (starts_with(line, "FORMAT="))
            {
                assert(starts_with(line, "FORMAT=32-bit_rle_rgbe"));
                format_found = 1;
            }
            else if (starts_with(line, "GAMMA="))
            {
                /* ignore gamma */
            }
            else if (starts_with(line, "EXPOSURE="))
            {
                /* ignore exposure */
            }
            else if (starts_with(line, "#"))
            {
                /* ignore comments */
            }
            else
            {
                assert(0);
            }
        }
        assert(header_found != 0);
        assert(format_found != 0);

        read_line(file, line);
        items = sscanf(line, "-Y %d +X %d", height, width);
        assert(items == 2 && width > 0 && height > 0);
    }

    void image_t::read_hdr_flat_scanline(FILE *file, int row)
    {
        for (int i = 0; i < width; i++)
        {
            float *pixel = get_hdr_pixel(row, i);
            unsigned char rgbe[4];
            read_bytes(file, rgbe, 4);
            rgbe_to_floats(rgbe, pixel);
        }
    }

    void image_t::read_hdr_rle_scanline(FILE *file, int row)
    {
        unsigned char chan[4][width];
        for (int i = 0; i < 4; ++i)
        {
            int size = 0;
            while (size < width)
            {
                unsigned char byte = read_byte(file);
                if (byte > 128)
                {
                    int count = byte - 128;
                    unsigned char value = read_byte(file);
                    assert(count > 0 && size + count <= width);
                    for (int j = 0; j < count; ++j)
                    {
                        chan[i][size++] = value;
                    }
                }
                else
                {
                    int count = byte;
                    assert(count > 0 && size + count <= width);
                    for (int j = 0; j < count; ++j)
                    {
                        chan[i][size++] = read_byte(file);
                    }
                }
            }
            assert(size == width);
        }
        for (int i = 0; i < width; i++)
        {
            float *pixel = get_hdr_pixel(row, i);
            unsigned char rgbe[4];
            for (int j = 0; j < 4; ++j)
            {
                rgbe[j] = chan[j][i];
            }
            rgbe_to_floats(rgbe, pixel);
        }
    }

    void image_t::read_hdr_scanline(FILE *file, int row)
    {
        if (width < 8 || width > 0x7fff)
        {
            read_hdr_flat_scanline(file, row);
        }
        else
        {
            unsigned char bytes[4];
            read_bytes(file, bytes, 4);
            if (bytes[0] != 2 || bytes[1] != 2 || bytes[2] & 0x80)
            {
                fseek(file, -4, SEEK_CUR);
                read_hdr_flat_scanline(file, row);
            }
            else
            {
                assert(bytes[2] * 256 + bytes[3] == width);
                read_hdr_rle_scanline(file, row);
            }
        }
    }

    void image_t::load_hdr_image(const char *filename)
    {

        FILE *file;
        int i;

        file = fopen(filename, "rb");
        assert(file != NULL);
        read_hdr_header(file, width, height);
        this->channels = 3;
        this->format = FORMAT_HDR;
        for (i = 0; i < height; i++)
        {
            int row = height - 1 - i;
            read_hdr_scanline(file, row);
        }
        fclose(file);
    }
    void image_t::image_save(const char *filename)
    {
        const char *extension = private_get_extension(filename);
        if (strcmp(extension, "tga") == 0)
        {
            save_tga_image(filename);
        }
        else if (strcmp(extension, "hdr") == 0)
        {
            save_hdr_image(filename);
        }
        else
        {
            assert(0);
        }
    }
    void image_t::save_tga_image(const char *filename)
    {
        unsigned char header[TGA_HEADER_SIZE];
        FILE *file;

        assert(format == FORMAT_LDR);

        file = fopen(filename, "wb");
        assert(file != NULL);

        memset(header, 0, TGA_HEADER_SIZE);
        header[2] = channels == 1 ? 3 : 2;  /* image type */
        header[12] = width & 0xFF;          /* width, lsb */
        header[13] = (width >> 8) & 0xFF;   /* width, msb */
        header[14] = height & 0xFF;         /* height, lsb */
        header[15] = (height >> 8) & 0xFF;  /* height, msb */
        header[16] = (channels * 8) & 0xFF; /* image depth */
        write_bytes(file, header, TGA_HEADER_SIZE);

        if (channels >= 3)
        {
            int r, c;
            for (r = 0; r < height; r++)
            {
                for (c = 0; c < width; c++)
                {
                    unsigned char *pixel = get_ldr_pixel(r, c);
                    unsigned char channels_arr[4];
                    memcpy(channels_arr, pixel, channels);
                    swap_bytes(&channels_arr[0], &channels_arr[2]); /* rgb to bgr */
                    write_bytes(file, channels_arr, channels);
                }
            }
        }
        else
        {
            write_bytes(file, ldr_buffer, get_num_elems());
        }

        fclose(file);
    }
    void image_t::save_hdr_image(const char *filename)
    {
        FILE *file;
        int r, c;

        assert(format == FORMAT_HDR && channels >= 3);

        file = fopen(filename, "wb");
        assert(file != NULL);

        fputs("#?RADIANCE\n", file);
        fputs("FORMAT=32-bit_rle_rgbe\n", file);
        fputs("\n", file);
        fprintf(file, "-Y %d +X %d\n", height, width);

        for (r = 0; r < height; r++)
        {
            for (c = 0; c < width; c++)
            {
                int flipped_r = height - 1 - r;
                float *pixel = get_hdr_pixel( flipped_r, c);
                unsigned char rgbe[4];
                floats_to_rgbe(pixel, rgbe);
                write_bytes(file, rgbe, 4);
            }
        }

        fclose(file);
    }
    void image_t::image_flip_h()
    {
        int half_width = width / 2;
        int r, c, k;
        for (r = 0; r < height; r++)
        {
            for (c = 0; c < half_width; c++)
            {
                int flipped_c = width - 1 - c;
                if (format == FORMAT_LDR)
                {
                    unsigned char *pixel1 = get_ldr_pixel(r, c);
                    unsigned char *pixel2 = get_ldr_pixel(r, flipped_c);
                    for (k = 0; k < channels; k++)
                    {
                        swap_bytes(&pixel1[k], &pixel2[k]);
                    }
                }
                else
                {
                    float *pixel1 = get_hdr_pixel(r, c);
                    float *pixel2 = get_hdr_pixel(r, flipped_c);
                    for (k = 0; k < channels; k++)
                    {
                        swap_floats(&pixel1[k], &pixel2[k]);
                    }
                }
            }
        }
    }
    void image_t::image_flip_v()
    {
        int half_height = height / 2;
        int r, c, k;
        for (r = 0; r < half_height; r++)
        {
            for (c = 0; c < width; c++)
            {
                int flipped_r = height - 1 - r;
                if (format == FORMAT_LDR)
                {
                    unsigned char *pixel1 = get_ldr_pixel(r, c);
                    unsigned char *pixel2 = get_ldr_pixel(flipped_r, c);
                    for (k = 0; k < channels; k++)
                    {
                        swap_bytes(&pixel1[k], &pixel2[k]);
                    }
                }
                else
                {
                    float *pixel1 = get_hdr_pixel(r, c);
                    float *pixel2 = get_hdr_pixel(flipped_r, c);
                    for (k = 0; k < channels; k++)
                    {
                        swap_floats(&pixel1[k], &pixel2[k]);
                    }
                }
            }
        }
    }
    unsigned char *image_t::get_ldr_pixel(int row, int col)
    {
        int index = (row * width + col) * channels;
        return &ldr_buffer[index];
    }
    float *image_t::get_hdr_pixel(int row, int col)
    {
        int index = (row * width + col) * channels;
        return &hdr_buffer[index];
    }
    int image_t::get_num_elems()
    {
        return width * height * channels;
    }
    image_t::~image_t()
    {
        if (ldr_buffer != nullptr)
        {
            delete[] ldr_buffer;
            ldr_buffer = nullptr;
        }
        if (hdr_buffer != nullptr)
        {
            delete[] hdr_buffer;
            hdr_buffer = nullptr;
        }
    }
} // namespace core
