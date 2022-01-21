#version 330 core

in vec3 f_pos;
in float f_coeff;
out vec4 FragColor;

void main() {
  vec3 N;
  N.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
  float dist = dot(N.xy, N.xy);
  if (dist > 1.0)
    return;
  vec3 color1 = vec3(0.000f, 0.545f, 0.800f);
  vec3 color2 = vec3(1.000f, 0.545f, 0.800f);
  FragColor = vec4(exp(-dist * dist) * mix(color1, color2, f_coeff), 1.0f);
  // FragColor = vec4(mix(color1, color2, f_coeff), 1.0f);
}