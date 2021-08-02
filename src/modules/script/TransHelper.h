#pragma once

#include <SM_Vector.h>

#include <vector>

namespace tt
{

std::vector<sm::vec2> list_to_vec2_array(int index);
std::vector<sm::vec3> list_to_vec3_array(int index);
std::vector<int> list_to_int_array(int index);

uint32_t list_to_abgr(int index);
uint32_t list_to_rgba(int index);

sm::vec2 list_to_vec2(int index);
sm::vec3 list_to_vec3(int index);
sm::vec4 list_to_vec4(int index);

}