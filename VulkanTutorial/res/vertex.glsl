#version 450
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fColor;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    fColor = color;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 0.0, 1.0);
}