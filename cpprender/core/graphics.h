#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "maths.h"
#include <memory>
namespace core{

class framebuffer_t{
private:
    int width, height;
public:    
    void set_width(int _width);
    int get_width();

    void set_height(int _height);
    int get_height();

    framebuffer_t(int _width,int _height);
    ~framebuffer_t();
    framebuffer_t(const framebuffer_t&)=delete;
    framebuffer_t operator=(const framebuffer_t&)=delete;

    void clear_color(vec4_t _color);
    void clear_depth(float _clear);

    std::unique_ptr<unsigned char[]> color_buffer;
    std::unique_ptr<float[]> depth_buffer;
};


/* program management */

#define MAX_VARYINGS 10

template <typename _Attribs,typename _Varyings,typename _Uniforms>
struct program_t {
    using vertex_shader_t=vec4_t(*)(_Attribs,_Varyings,_Uniforms);
    using fragment_shader_t=vec4_t(*)(_Varyings,_Uniforms,int&,int);
    vertex_shader_t vertex_shader;
    fragment_shader_t fragment_shader;

    int double_sided;
    int enable_blend;
    /* for shaders */
    _Attribs  shader_attribs[3];
    _Varyings shader_varyings;
    _Uniforms shader_uniforms;
    /* for clipping */
    vec4_t in_coords[MAX_VARYINGS];
    vec4_t out_coords[MAX_VARYINGS];
    _Varyings in_varyings[MAX_VARYINGS];
    _Varyings out_varyings[MAX_VARYINGS];

    program_t(vertex_shader_t,fragment_shader_t,int double_sided,int enable_blend);
    _Attribs get_attribs(int nth_vertex);
    _Uniforms get_uniforms();

    program_t(const program_t&)=delete;
    program_t operator=(const program_t&)=delete;
    
};


/* graphics pipeline */
template<typename _Attribs, typename _Varyings,typename _Uniforms>
void graphics_draw_triangle(framebuffer_t& framebuffer, program_t<_Attribs,_Varyings,_Uniforms>& program);



} // core 
#endif //_GRAPHICS_H
