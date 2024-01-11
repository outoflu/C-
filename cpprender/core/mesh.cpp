#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.hpp"
#include "mesh.hpp"
#include "private.hpp"

using std::vector;

namespace core
{
    mesh_t::mesh_t():num_faces(0),vertices(nullptr)
    {
        center={0,0,0};
    }
    mesh_t::mesh_t(const char *filename)
    {
        mesh_load(filename);
    }
    mesh_t::~mesh_t()
    {
        if (vertices!=nullptr){
            delete[] vertices;
        }
    }
    int mesh_t::mesh_get_num_faces() const
    {
        return this->num_faces;
    }
    vertex_t *mesh_t::mesh_get_vertices() const
    {
        return this->vertices;
    }
    vec3_t mesh_t::mesh_get_center() const
    {
        return this->center;
    }
    void mesh_t::mesh_load(const char *filename)
    {
        const char *extension = private_get_extension(filename);
        if (strcmp(extension, "obj") == 0)
        {
            load_obj(filename);
        }
        else
        {
            assert(0);
        }
    }
    void mesh_t::load_obj(const char *filename)
    {
        vector<vec3_t> positions;
        vector<vec2_t> texcoords;
        vector<vec3_t> normals;
        vector<vec4_t> tangents;
        vector<vec4_t> joints;
        vector<vec4_t> weights;
        vector<int> position_indices;
        vector<int> texcoord_indices;
        vector<int> normal_indices;
        char line[LINE_SIZE];
        FILE *file = fopen(filename, "rb");
        assert(file != nullptr);
        while (1)
        {
            int items;
            if (fgets(line, LINE_SIZE, file) == NULL)
            {
                break;
            }
            else if (strncmp(line, "v ", 2) == 0)
            { /* position */
                vec3_t position;
                items = sscanf(line, "v %f %f %f",
                               &position.x, &position.y, &position.z);
                assert(items == 3);
                positions.push_back(position);
            }
            else if (strncmp(line, "vt ", 3) == 0)
            { /* texcoord */
                vec2_t texcoord;
                items = sscanf(line, "vt %f %f",
                               &texcoord.x, &texcoord.y);
                assert(items == 2);
                texcoords.push_back(texcoord);
            }
            else if (strncmp(line, "vn ", 3) == 0)
            { /* normal */
                vec3_t normal;
                items = sscanf(line, "vn %f %f %f",
                               &normal.x, &normal.y, &normal.z);
                assert(items == 3);
                normals.push_back(normal);
            }
            else if (strncmp(line, "f ", 2) == 0)
            { /* face */
                int i;
                int pos_indices[3], uv_indices[3], n_indices[3];
                items = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                               &pos_indices[0], &uv_indices[0], &n_indices[0],
                               &pos_indices[1], &uv_indices[1], &n_indices[1],
                               &pos_indices[2], &uv_indices[2], &n_indices[2]);
                assert(items == 9);
                for (i = 0; i < 3; i++)
                {
                    position_indices.push_back(pos_indices[i] - 1);
                    texcoord_indices.push_back(uv_indices[i] - 1);
                    normal_indices.push_back(n_indices[i] - 1);
                }
            }
            else if (strncmp(line, "# ext.tangent ", 14) == 0)
            { /* tangent */
                vec4_t tangent;
                items = sscanf(line, "# ext.tangent %f %f %f %f",
                               &tangent.x, &tangent.y, &tangent.z, &tangent.w);
                assert(items == 4);
                tangents.push_back(tangent);
            }
            else if (strncmp(line, "# ext.joint ", 12) == 0)
            { /* joint */
                vec4_t joint;
                items = sscanf(line, "# ext.joint %f %f %f %f",
                               &joint.x, &joint.y, &joint.z, &joint.w);
                assert(items == 4);
                joints.push_back(joint);
            }
            else if (strncmp(line, "# ext.weight ", 13) == 0)
            { /* weight */
                vec4_t weight;
                items = sscanf(line, "# ext.weight %f %f %f %f",
                               &weight.x, &weight.y, &weight.z, &weight.w);
                assert(items == 4);
                weights.push_back(weight);
            }
        }
        fclose(file);
        build_mesh(positions,texcoords,normals,tangents,joints,weights,position_indices,texcoord_indices,normal_indices);
    }
    void mesh_t::build_mesh(const vector<vec3_t> &positions, const vector<vec2_t> &texcoords, const vector<vec3_t> &normals, const vector<vec4_t> &tangents, const vector<vec4_t> &joints, const vector<vec4_t> &weights, const vector<int> &position_indices,const vector<int> &texcoord_indices, const vector<int> &normal_indices)
    {
        vec3_t bbox_min = vec3_new(+1e6, +1e6, +1e6);
        vec3_t bbox_max = vec3_new(-1e6, -1e6, -1e6);
        int num_indices = position_indices.size();
        int _num_faces = num_indices / 3;

        assert(_num_faces > 0 && _num_faces * 3 == num_indices);
        assert(position_indices.size() == num_indices);
        assert(texcoord_indices.size() == num_indices);
        assert(normal_indices.size() == num_indices);

        this->vertices = new vertex_t[num_indices];
        for (int i = 0; i < num_indices; i++)
        {
            int position_index = position_indices[i];
            int texcoord_index = texcoord_indices[i];
            int normal_index = normal_indices[i];
            assert(position_index >= 0 && position_index < num_indices);
            assert(texcoord_index >= 0 && texcoord_index < num_indices);
            assert(normal_index >= 0 && normal_index < num_indices);
            vertices[i].position = positions[position_index];
            vertices[i].texcoord = texcoords[texcoord_index];
            vertices[i].normal = normals[normal_index];
            if (tangents.size()){
                int tangent_index=position_index;
                assert(tangent_index>=0&&tangent_index<tangents.size());
                vertices[i].tangent=tangents[tangent_index];
            } else {
                vertices[i].tangent={1,0,0,1};
            }

            if (joints.size()){
                int joint_index=position_index;
                assert(joint_index>=0&&joint_index<joints.size());
                vertices[i].joint=joints[joint_index];
            } else {
                vertices[i].joint={0,0,0,0};
            }
            
            if (weights.size()){
                int weight_index=position_index;
                assert(weight_index>=0&&weight_index<weights.size());
                vertices[i].weight=weights[weight_index];
            } else {
                vertices[i].weight={0,0,0,0};
            }

            bbox_min=vec3_min(bbox_min,vertices[i].position);
            bbox_max=vec3_max(bbox_max,vertices[i].position);
        }
        this->num_faces=_num_faces;
        this->center=vec3_div(vec3_add(bbox_max,bbox_min),2);
    }

}