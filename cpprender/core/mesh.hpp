#ifndef _MESH_H
#define _MESH_H
#include <vector>
#include "maths.hpp"

namespace core{
    struct vertex_t{
        vec3_t position;
        vec2_t texcoord;
        vec3_t normal;
        vec4_t tangent;
        vec4_t joint;
        vec4_t weight;
    };

    class mesh_t{
    public:
        int num_faces;
        vertex_t* vertices;
        vec3_t center;
        mesh_t();
        mesh_t(const char* filename);
        mesh_t(const mesh_t&)=delete;
        mesh_t operator=(const mesh_t&)=delete;
        ~mesh_t();
        int mesh_get_num_faces() const;
        vertex_t* mesh_get_vertices() const;
        vec3_t mesh_get_center() const;
    private:
        void mesh_load(const char* filename);
        void load_obj(const char* filename);
        void build_mesh(const std::vector<vec3_t> &positions,const std::vector<vec2_t> &texcoords,const std::vector<vec3_t> &normals,
                    const std::vector<vec4_t> &tangents,const std::vector<vec4_t> &joints,const std::vector<vec4_t> &weights,
                    const std::vector<int> &position_indices,const std::vector<int> &texcoord_indices,const std::vector<int> &normal_indices);
    };
}

#endif //_MESH_H