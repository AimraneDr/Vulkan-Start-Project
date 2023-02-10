#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

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
  mat4 normalMatrix;
} push;


void main(){
  vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
  vec3 specularLight = vec3(0.0);
  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.inverseView[3].xyz;
  vec3 viewDir = normalize(cameraPosWorld - fragPosWorld);

  for (int i = 0; i < ubo.numLights; i++) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squered

    directionToLight = normalize(directionToLight);
    float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosAngIncidence;

    //specular liight
    vec3 halfAng = normalize(directionToLight + viewDir);
    float blinnTerm = dot(surfaceNormal, halfAng);
    blinnTerm = clamp(blinnTerm,0,1);
    blinnTerm = pow(blinnTerm, 1132.0);
    specularLight += light.color.xyz * attenuation * blinnTerm;
  }


  
  // float directionalLightIntensity = ubo.ambient + max(dot(normalize(fragNormalWorld), ubo.directionalLight.xyz),0);
  // outColor = vec4(diffuseLight * fragColor + specularLight * fragColor + directionalLightIntensity * fragColor , 1.0);

  outColor = vec4(diffuseLight * fragColor + specularLight * fragColor , 1.0);
}