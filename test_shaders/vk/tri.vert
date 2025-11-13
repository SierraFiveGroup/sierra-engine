#version 450

//layout(location = 0) in vec2 aVert;

vec3 vertices[] = {
    {-0.5, -0.5, 0.0},
    {-0.5, 0.5, 0.0},
    {0.5, 0.5, 0.0},

    {-0.2, -0.2, 0.1},
    {-0.2, 0.8, 0.1},
    {0.8, 0.8, 0.1},
};

void main() {
    gl_Position = vec4(vertices[gl_VertexIndex], 1.0);
}