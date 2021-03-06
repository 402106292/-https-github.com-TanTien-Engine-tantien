#version 430

#include "includes.comp"
#include "layout_size.comp"

uniform UBO
{
	ivec2 spotPos;
	vec3 color;
	float intensity;	
};

layout(rgba16f, binding = 0) uniform image2D field;

void main()
{
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
  vec2 p = vec2(pixelCoords - spotPos);

  vec3 splat = intensity * exp(- dot(p, p) / 200.0f) * color;
  vec3 baseD = imageLoad(field, pixelCoords).xyz;

  imageStore(field, pixelCoords, vec4(baseD + splat, 1.0f));
}
