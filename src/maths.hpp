
#pragma once

#include "maths_types.hpp"

#include <cmath>


constexpr float PI{float{M_PI}};
constexpr float TWO_PI{2.0f * float{M_PI}};

float degrees(float radians);
float radians(float degrees);


float get_angle(vec2 const &v);
float get_length(vec2 const &v);

vec2 angle_to_vec2(float angle, float length);

vec2 operator+(vec2 const &a, vec2 const &b);
vec2 operator-(vec2 const &a, vec2 const &b);
vec2 operator*(vec2 const &a, float s);
vec2 operator/(vec2 const &a, float s);

vec2 &operator+=(vec2 &a, vec2 const &b);
vec2 &operator-=(vec2 &a, vec2 const &b);
vec2 &operator*=(vec2 &a, float s);
vec2 &operator/=(vec2 &a, float s);

col4 operator*(col4 const &a, float s);

vec2 vec_abs(vec2 const &v);

float dot(vec2 const &a, vec2 const &b);
float distance(vec2 const &v1, vec2 const &v2);
float distance_squared(vec2 const &v1, vec2 const &v2);

vec2 get_normal(vec2 const &p1, vec2 const &p2);
vec2 normalize(vec2 const &v);
vec2 reflect(vec2 const &incident, vec2 const &normal);

vec2 nearest_point_on_line(vec2 const &v, vec2 const &w, vec2 const &p);
vec2 nearest_point_on_line_segment(vec2 const &v, vec2 const &w, vec2 const &p);
vec2 get_intersection(vec2 ps1, vec2 pe1, vec2 ps2, vec2 pe2); //Throws

bool in_range(float beg, float end, float p);
float clamp(float min, float max, float val);


float RandomFloat();
float RandomFloat(const float r1, const float r2);
int RandomInt(int r1, int r2);
col4 RandomRGB();
col4 RandomRGBA();


mat4 mat4_identity();
mat4 mat4_zero();
