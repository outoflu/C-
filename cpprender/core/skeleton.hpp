#ifndef _SKELETON_HPP
#define _SKELETON_HPP

#include "maths.hpp"
#include "private.hpp"
#include "macro.hpp"

namespace core
{
    class joint_t
    {
    public:
        int joint_index;
        int parent_index;
        mat4_t inverse_bind;
        /* translations */
        int num_translations;
        float *translation_times;
        vec3_t *translation_values;
        /* rotations */
        int num_rotations;
        float *rotation_times;
        quat_t *rotation_values;
        /* scales */
        int num_scales;
        float *scale_times;
        vec3_t *scale_values;
        /* interpolated */
        mat4_t transform;
        void load_joint(FILE* file);
    private:
        void read_inverse_bind(FILE* file);
        void read_translations(FILE* file);
        void read_rotations(FILE* file);
        void read_scales(FILE* file);
    public:
        joint_t();
        joint_t(FILE* file);
        ~joint_t();
        vec3_t get_translation(float frame_time)const;
        quat_t get_rotation(float frame_time)const;
        vec3_t get_scale(float frame_time) const;
    };

    class skeleton_t
    {
    public:
        float min_time;
        float max_time;
        int num_joints;
        joint_t *joints;
        /* cached result */
        mat4_t *joint_matrices;
        mat3_t *normal_matrices;
        float last_time;
        skeleton_t(const char* filename);
    private:
        void initialize_cache();
        void skeleton_load(const char* filename);
        void skeleton_update_joints(float frame_time);
        mat4_t* skeleton_get_joint_matrices();
        mat3_t* skeleton_get_normal_matrices();
    public:
        ~skeleton_t();
    private:
        void load_ani(const char* filename);
    };

} // core
#endif //_SKELETON_HPP