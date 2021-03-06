#version 450

#include "includes.comp"
#include "layout_size.comp"

uniform UBO
{
  float dt;
  float revert;  
};

layout(rgba16f, binding = 0) uniform image2D field_WRITE;
layout(binding = 1) uniform sampler2D field_n;
layout(binding = 2) uniform sampler2D field_n_hat_READ;
layout(binding = 3) uniform sampler2D field_n_1_READ;
layout(binding = 4) uniform sampler2D velocities_READ;

void main()
{
  vec2 tSize = TEXTURE_SIZE(field_n_hat_READ);
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

  vec4 qAdv = texelFetch(field_n_1_READ, pixelCoords, 0);

  vec4 r = qAdv + 0.5 * texelFetch(field_n, pixelCoords, 0)
    - 0.5 * texelFetch(field_n_hat_READ, pixelCoords, 0);

  vec2 v = RK(velocities_READ, pixelCoords, dt);
  vec2 pos = pixelCoords - dt * v;
  vec4 rClamped = clampValue(field_n, r, pixelToTexel(pos, tSize));

  r = length(rClamped - r) > revert ? qAdv : rClamped;

  imageStore(field_WRITE, pixelCoords, r);
}
