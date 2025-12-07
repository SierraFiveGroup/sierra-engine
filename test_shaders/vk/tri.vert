#version 450

layout(location = 0) in vec3 aVert;
layout(location = 0) out vec3 oVert;

//@UNIFORM_BUFFER 0 projection
layout(binding = 0) uniform Transform {
    mat4 proj;
} transform;


void main() {
    gl_Position = transform.proj * vec4(aVert.xy, (aVert.z - 1) / 2, 1.0);
    oVert = aVert;
}