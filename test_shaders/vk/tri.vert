#version 450

layout(location = 0) in vec3 aVert;

//@UNIFORM_BUFFER 0 projection //uhhhhhhh
layout(binding = 0) uniform Transform {
    mat4 proj;
} transform;

void main() {
    gl_Position = transform.proj * vec4(aVert.xy, 0.0, 1.0);
}