#version 430

#include "includes.comp"
#include "layout_size.comp"

uniform UBO
{
	float dt;
	float kappa;
	float sigma;
	float t0;	
};

layout(rgba16f, binding = 0) uniform image2D velocities_READ_WRITE;
layout(binding = 1) uniform sampler2D temperature;
layout(binding = 2) uniform sampler2D density;

void main()
{
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

  float t = texelFetch(temperature, pixelCoords, 0).x;
  float d = texelFetch(density, pixelCoords, 0).x;

  vec2 force = (- kappa * d + sigma * (t - t0)) * vec2(0.0f, 1.0f);
  vec4 oldVel = imageLoad(velocities_READ_WRITE, pixelCoords);

  imageStore(velocities_READ_WRITE, pixelCoords, oldVel + dt * vec4(force, 0.0f, 0.0f));
}
