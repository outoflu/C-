#include "skeleton.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

namespace core
{
    void joint_t::load_joint(FILE *file)
    {
        int items;
        items = fscanf(file, " joint %d:", &joint_index);
        assert(items == 1);
        items = fscanf(file, " parent-index: %d", &parent_index);
        assert(items == 1);

        read_inverse_bind(file);
        read_translations(file);
        read_rotations(file);
        read_scales(file);
    }
    void joint_t::read_inverse_bind(FILE *file)
    {
        char line[LINE_SIZE];
        int items = fscanf(file, " %s", line);
        assert(items == 1 && strcmp(line, "inverse-bind:") == 0);
        for (int i = 0; i < 4; ++i)
        {
            items = fscanf(file, " %f %f %f %f",
                           &inverse_bind.m[i][0],
                           &inverse_bind.m[i][1],
                           &inverse_bind.m[i][2],
                           &inverse_bind.m[i][3]);
            assert(items == 4);
        }
    }
    void joint_t::read_translations(FILE *file)
    {
        int items;
        int i;
        items = fscanf(file, " translations %d:", &num_translations);
        assert(items == 1 && num_translations >= 0);
        if (num_translations > 0)
        {
            translation_times = new float[num_translations];
            translation_values = new vec3_t[num_translations];
            for (i = 0; i < num_translations; i++)
            {
                items = fscanf(file, " time: %f, value: [%f, %f, %f]",
                               &translation_times[i],
                               &translation_values[i].x,
                               &translation_values[i].y,
                               &translation_values[i].z);
                assert(items == 4);
            }
        }
        else
        {
            translation_times = nullptr;
            translation_values = nullptr;
        }
    }
    void joint_t::read_rotations(FILE *file)
    {
        int i;
        int items = fscanf(file, " rotations %d:", &num_rotations);
        assert(items == 1 && num_rotations >= 0);
        if (num_rotations > 0)
        {
            rotation_times = new float[num_rotations];
            rotation_values = new quat_t[num_rotations];
            for (i = 0; i < num_rotations; i++)
            {
                items = fscanf(file, " time: %f, value: [%f, %f, %f, %f]",
                               &rotation_times[i],
                               &rotation_values[i].x,
                               &rotation_values[i].y,
                               &rotation_values[i].z,
                               &rotation_values[i].w);
                assert(items == 5);
            }
        }
        else
        {
            rotation_times = nullptr;
            rotation_values = nullptr;
        }
    }

    void joint_t::read_scales(FILE *file)
    {
        int items;
        int i;
        items = fscanf(file, " scales %d:", &num_scales);
        assert(items == 1 && num_scales >= 0);
        if (num_scales > 0)
        {
            scale_times = new float[num_scales];
            scale_values = new vec3_t[num_scales];
            for (i = 0; i < num_scales; i++)
            {
                items = fscanf(file, " time: %f, value: [%f, %f, %f]",
                               &scale_times[i],
                               &scale_values[i].x,
                               &scale_values[i].y,
                               &scale_values[i].z);
                assert(items == 4);
            }
        }
        else
        {
            scale_times = nullptr;
            scale_values = nullptr;
        }
    }

    joint_t::joint_t() : translation_times(nullptr), translation_values(nullptr),
                         rotation_times(nullptr), rotation_values(nullptr),
                         scale_times(nullptr), scale_values(nullptr)
    {
    }

    joint_t::joint_t(FILE *file)
    {
        load_joint(file);
    }

    joint_t::~joint_t()
    {
        delete[] translation_times;
        delete[] translation_values;
        delete[] rotation_times;
        delete[] rotation_values;
        delete[] scale_times;
        delete[] scale_values;
    }

    vec3_t joint_t::get_translation(float frame_time) const
    {
        if (num_translations == 0)
        {
            return {0, 0, 0};
        }
        else if (frame_time <= translation_times[0])
        {
            return translation_values[0];
        }
        else if (frame_time >= translation_times[num_translations - 1])
        {
            return translation_values[num_translations - 1];
        }
        else
        {
            for (int i = 0; i < num_translations - 1; ++i)
            {
                float curr_time = translation_times[i];
                float next_time = translation_times[i + 1];
                if (frame_time >= curr_time && frame_time < next_time)
                {
                    float t = (frame_time - curr_time) / (next_time - curr_time);
                    vec3_t curr_translation = translation_values[i];
                    vec3_t next_translation = translation_values[i + 1];
                    return vec3_lerp(curr_translation, next_translation, t);
                }
            }
        }
    }

    quat_t joint_t::get_rotation(float frame_time) const
    {
        if (num_rotations == 0)
        {
            return quat_new(0, 0, 0, 1);
        }
        else if (frame_time <= rotation_times[0])
        {
            return rotation_values[0];
        }
        else if (frame_time >= rotation_times[num_rotations - 1])
        {
            return rotation_values[num_rotations - 1];
        }
        else
        {
            int i;
            for (i = 0; i < num_rotations - 1; i++)
            {
                float curr_time = rotation_times[i];
                float next_time = rotation_times[i + 1];
                if (frame_time >= curr_time && frame_time < next_time)
                {
                    float t = (frame_time - curr_time) / (next_time - curr_time);
                    quat_t curr_rotation = rotation_values[i];
                    quat_t next_rotation = rotation_values[i + 1];
                    return quat_slerp(curr_rotation, next_rotation, t);
                }
            }
            assert(0);
            return quat_new(0, 0, 0, 1);
        }
    }

    vec3_t joint_t::get_scale(float frame_time) const
    {
        if (num_scales == 0)
        {
            return vec3_new(1, 1, 1);
        }
        else if (frame_time <= scale_times[0])
        {
            return scale_values[0];
        }
        else if (frame_time >= scale_times[num_scales - 1])
        {
            return scale_values[num_scales - 1];
        }
        else
        {
            int i;
            for (i = 0; i < num_scales - 1; i++)
            {
                float curr_time = scale_times[i];
                float next_time = scale_times[i + 1];
                if (frame_time >= curr_time && frame_time < next_time)
                {
                    float t = (frame_time - curr_time) / (next_time - curr_time);
                    vec3_t curr_scale = scale_values[i];
                    vec3_t next_scale = scale_values[i + 1];
                    return vec3_lerp(curr_scale, next_scale, t);
                }
            }
            assert(0);
            return vec3_new(1, 1, 1);
        }
    }

    skeleton_t::skeleton_t(const char *filename)
    {
        skeleton_load(filename);
        initialize_cache();
    }
    void skeleton_t::initialize_cache()
    {
        joint_matrices = new mat4_t[num_joints];
        normal_matrices = new mat3_t[num_joints];
        memset(joint_matrices, 0, sizeof(mat4_t) * num_joints);
        memset(normal_matrices, 0, sizeof(mat3_t) * num_joints);
        last_time = -1;
    }
    void skeleton_t::skeleton_load(const char *filename)
    {
        const char *extension = private_get_extension(filename);
        if (strcmp(extension, "ani") == 0)
        {
            load_ani(filename);
        }
        else
        {
            assert(0);
        }
    }
    void skeleton_t::skeleton_update_joints(float frame_time)
    {
        frame_time=(float)fmod(frame_time,max_time);
        if (frame_time!=last_time){
            for (int i=0;i<num_joints;++i){
                vec3_t translation=joints[i].get_translation(frame_time);
                quat_t rotation=joints[i].get_rotation(frame_time);
                vec3_t scale=joints[i].get_scale(frame_time);

                joints[i].transform=mat4_from_trs(translation,rotation,scale);
                if (joints[i].parent_index>=0){
                    joint_t* parent=&joints[joints[i].parent_index];
                    joints[i].transform=mat4_mul_mat4(parent->transform,joints[i].transform);
                }

                mat4_t joint_matrix=mat4_mul_mat4(joints[i].transform,joints[i].inverse_bind);
                mat3_t normal_matrix=mat3_inverse_transpose(mat3_from_mat4(joint_matrix));
                joint_matrices[i]=joint_matrix;
                normal_matrices[i]=normal_matrix;
            }
            last_time=frame_time;
        }
    }
    mat4_t *skeleton_t::skeleton_get_joint_matrices()
    {
        return joint_matrices;
    }
    mat3_t *skeleton_t::skeleton_get_normal_matrices()
    {
        return normal_matrices;
    }
    skeleton_t::~skeleton_t()
    {
        delete[] joints;
        delete[] joint_matrices;
        delete[] normal_matrices;
    }
    void skeleton_t::load_ani(const char *filename)
    {
        FILE *file;
        int items;
        int i;

        file = fopen(filename, "rb");
        assert(file != NULL);

        items = fscanf(file, " joint-size: %d", &num_joints);
        assert(items == 1 && num_joints > 0);
        items = fscanf(file, " time-range: [%f, %f]",
                       &min_time, &max_time);
        assert(items == 2 && min_time < max_time);

        joints = new joint_t[num_joints];
        for (i = 0; i < num_joints; i++)
        {
            joints[i].load_joint(file);
            assert(joints[i].joint_index == i);
        }
        fclose(file);
        initialize_cache();
    }
} // core