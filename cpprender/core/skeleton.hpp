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
    };

    class skeleton
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
        skeleton(const char* filename);
        void initialize_cache();
        void skeleton_load(const char* filename);
        void skeleton_update_joints(float frame_time);
        ~skeleton();
    private:
        void load_ani(const char* filename);
    };

} // core
#endif //_SKELETON_HPP