#ifndef _MATHS_H
#define _MATHS_H

namespace core{

struct vec2_t {float x, y;};
struct vec3_t {float x, y, z;};
struct vec4_t {float x, y, z, w;};
struct quat_t {float x, y, z, w;};
struct mat3_t {float m[3][3];};
struct mat4_t {float m[4][4];};

/* float related functions */
float float_min(float a, float b);
float float_max(float a, float b);
float float_lerp(float a, float b, float t);
float float_clamp(float f, float min, float max);
float float_saturate(float f);
float float_from_uchar(unsigned char value);
unsigned char float_to_uchar(float value);
float float_srgb2linear(float value);
float float_linear2srgb(float value);
float float_aces(float value);
void float_print(const char *name, float f);

/* vec2 related functions */
vec2_t vec2_new(float x, float y);
vec2_t vec2_min(const vec2_t& a, const vec2_t& b);
vec2_t vec2_max(const vec2_t& a, const vec2_t& b);
vec2_t vec2_add(const vec2_t& a, const vec2_t& b);
vec2_t vec2_sub(const vec2_t& a, const vec2_t& b);
vec2_t vec2_mul(const vec2_t& v, float factor);
vec2_t vec2_div(const vec2_t& v, float divisor);
float vec2_length(const vec2_t& v);
float vec2_edge(const vec2_t& start, const vec2_t& end, const vec2_t& v);
void vec2_print(const char *name, const vec2_t& v);

/* vec3 related functions */
vec3_t vec3_new(float x, float y, float z);
vec3_t vec3_from_vec4(const vec4_t& v);
vec3_t vec3_min(const vec3_t& a, const vec3_t& b);
vec3_t vec3_max(const vec3_t& a, const vec3_t& b);
vec3_t vec3_add(const vec3_t& a, const vec3_t& b);
vec3_t vec3_sub(const vec3_t& a, const vec3_t& b);
vec3_t vec3_mul(const vec3_t& v, float factor);
vec3_t vec3_div(const vec3_t& v, float divisor);
vec3_t vec3_negate(const vec3_t& v);
float vec3_length(const vec3_t& v);
vec3_t vec3_normalize(const vec3_t& v);
float vec3_dot(const vec3_t& a, const vec3_t& b);
vec3_t vec3_cross(const vec3_t& a, const vec3_t& b);
vec3_t vec3_lerp(const vec3_t& a, const vec3_t& b, float t);
vec3_t vec3_saturate(const vec3_t& v);
vec3_t vec3_modulate(const vec3_t& a, const vec3_t& b);
void vec3_print(const char *name, const vec3_t& v);

/* vec4 related functions */
vec4_t vec4_new(float x, float y, float z, float w); // 不需要修改，因为返回值是 vec4_t
vec4_t vec4_from_vec3(const vec3_t& v, float w); // 使用 const vec3_t & 替换 vec3_t
vec4_t vec4_add(const vec4_t& a, const vec4_t& b); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_sub(const vec4_t& a, const vec4_t& b); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_mul(const vec4_t& v, float factor); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_div(const vec4_t& v, float divisor); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_lerp(const vec4_t& a, const vec4_t& b, float t); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_saturate(const vec4_t& v); // 使用 const vec4_t & 替换 vec4_t
vec4_t vec4_modulate(const vec4_t& a, const vec4_t& b); // 使用 const vec4_t & 替换 vec4_t
void vec4_print(const char *name, const vec4_t& v); // 使用 const vec4_t & 替换 vec4_t


/* quat related functions */
quat_t quat_new(float x, float y, float z, float w); // 不需要修改，因为返回值是 quat_t
float quat_dot(const quat_t& a, const quat_t& b); // 使用 const quat_t & 替换 quat_t
float quat_length(const quat_t& q); // 使用 const quat_t & 替换 quat_t
quat_t quat_normalize(const quat_t& q); // 使用 const quat_t & 替换 quat_t
quat_t quat_slerp(const quat_t& a, const quat_t& b_in, float t); // 使用 const quat_t & 替换 quat_t
void quat_print(const char *name, const quat_t& q); // 使用 const quat_t & 替换 quat_t


/* mat3 related functions */
mat3_t mat3_identity(void); // 不需要修改，因为没有参数
mat3_t mat3_from_cols(const vec3_t& c0, const vec3_t& c1, const vec3_t& c2); // 使用 const vec3_t & 替换 vec3_t
mat3_t mat3_from_mat4(const mat4_t& m); // 使用 const mat4_t & 替换 mat4_t
mat3_t mat3_combine(const mat3_t m[4], const vec4_t& weights); // 使用 const mat3_t m[4] 和 const vec4_t & 替换 mat3_t m[4] 和 vec4_t
vec3_t mat3_mul_vec3(const mat3_t& m, const vec3_t& v); // 使用 const mat3_t & 和 const vec3_t & 替换 mat3_t 和 vec3_t
mat3_t mat3_mul_mat3(const mat3_t& a, const mat3_t& b); // 使用 const mat3_t & 替换 mat3_t
mat3_t mat3_inverse(const mat3_t& m); // 使用 const mat3_t & 替换 mat3_t
mat3_t mat3_transpose(const mat3_t& m); // 使用 const mat3_t & 替换 mat3_t
mat3_t mat3_inverse_transpose(const mat3_t& m); // 使用 const mat3_t & 替换 mat3_t
void mat3_print(const char *name, const mat3_t& m); // 使用 const mat3_t & 替换 mat3_t

/* mat4 related functions */
mat4_t mat4_identity(void);
mat4_t mat4_from_quat(const quat_t& q);
mat4_t mat4_from_trs(const vec3_t& t, const quat_t& r, const vec3_t& s);
mat4_t mat4_combine(const mat4_t m[4],const vec4_t& weights);
vec4_t mat4_mul_vec4(const mat4_t& m, const vec4_t& v);
mat4_t mat4_mul_mat4(const mat4_t& a, const mat4_t& b);
mat4_t mat4_inverse(const mat4_t& m);
mat4_t mat4_transpose(const mat4_t& m);
mat4_t mat4_inverse_transpose(const mat4_t& m);
void mat4_print(const char *name, const mat4_t& m);

/* transformation matrices */
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate(float angle, float vx, float vy, float vz);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t mat4_rotate_z(float angle);
mat4_t mat4_lookat(vec3_t eye, vec3_t target, vec3_t up);
mat4_t mat4_ortho(float left, float right, float bottom, float top,
                  float near, float far);
mat4_t mat4_frustum(float left, float right, float bottom, float top,
                    float near, float far);
mat4_t mat4_orthographic(float right, float top, float near, float far);
mat4_t mat4_perspective(float fovy, float aspect, float near, float far);

} //core 
#endif //_MATHS_H
