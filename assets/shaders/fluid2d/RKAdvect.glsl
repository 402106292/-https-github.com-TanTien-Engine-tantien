#version 450

#include "includes.comp"
#include "layout_size.comp"

uniform UBO
{
	float dt;	
};

layout(rgba16f, binding = 0) uniform image2D field_WRITE;
layout(binding = 1) uniform sampler2D field_READ;
layout(binding = 2) uniform sampler2D velocities_READ;

void main()
{
  vec2 tSize = TEXTURE_SIZE(field_READ);
  vec2 pixelCoords = gl_GlobalInvocationID.xy;

  vec2 v = RK(velocities_READ, pixelCoords, dt);
  vec2 pos = pixelCoords - dt * v;
  vec4 val = TEXTURE_2D(field_READ, pixelToTexel(pos, tSize));

  imageStore(field_WRITE, ivec2(pixelCoords), val);
}
