#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec4 fragColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  PointLight pointLights[10];
  vec4 directionalLight;
  vec4 ambientLightColor; // w is intensity
  int numLights;
  float ambient;
} ubo;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
} push;

const vec3 DIRECTIONAL_LIGHT = normalize(vec3(1.0, -3.0 ,-1.0));
const float AMBIENT = 0.05;

void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    vec3 normalWorld = normalize(mat3(push.modelMatrix) * normal);
    float intensity = AMBIENT + max(dot(normalWorld, DIRECTIONAL_LIGHT),0);
    gl_Position = ubo.projection * ubo.view * positionWorld;
    fragColor = vec4(0.5, 0.5, 0.5, 1.0) * intensity;
}