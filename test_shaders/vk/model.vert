#version 450

layout(location = 0) in vec3 aVert;

layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aRotation;
layout(location = 3) in vec3 aScale;


layout(location = 4) in vec3 aTexCoord;

layout(location = 0) out vec3 oTexCoord;

//UNIFORM_BUFFER 0 projection
/*layout(binding = 0) uniform Transform {
    mat4 proj;
} transform;
*/

void main() {
    //gl_Position = transform.proj * vec4(aVert.xy, (aVert.z - 1) / 2, 1.0);
    oTexCoord = aTexCoord;
}