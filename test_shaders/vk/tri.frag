#version 450

//@COMBINED_IMAGE_SAMPLER 1 img
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 texCoord;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, texCoord.xy);
}
