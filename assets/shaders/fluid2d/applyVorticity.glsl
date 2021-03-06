#version 430

#include "includes.comp"
#include "layout_size.comp"

uniform UBO
{
  float dt;  
};

layout(rgba16f, binding = 0) uniform image2D velocities_READ_WRITE;
layout(binding = 1) uniform sampler2D curl;

void main()
{
  const ivec2 tSize = TEXTURE_SIZE(curl);
  const ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
  const ivec2 dx = ivec2(1, 0);
  const ivec2 dy = ivec2(0, 1);

  float vL = texelFetchOffset(curl, pixelCoords, 0, - dx).y;
  float vR = texelFetchOffset(curl, pixelCoords, 0,   dx).y;
  float vB = texelFetchOffset(curl, pixelCoords, 0, - dy).y;
  float vT = texelFetchOffset(curl, pixelCoords, 0,   dy).y;

  float vC = texelFetch(curl, pixelCoords, 0).y;
  if(pixelCoords.x == 0) vL = vC;
  if(pixelCoords.y == 0) vB = vC;
  if(pixelCoords.x >= tSize.x - 1) vR = vC;
  if(pixelCoords.y >= tSize.y - 1) vT = vC;

  vec2 force = 0.5f * vec2(abs(vT) - abs(vB), abs(vR) - abs(vL));
  force /= 1e-10 + length(force);
  force *= vC * vec2(1.0f, -1.0f);

  imageStore(velocities_READ_WRITE, pixelCoords, imageLoad(velocities_READ_WRITE, pixelCoords) + dt * vec4(force, 0.0f, 0.0f));
}
