#version 450

//layout(location = 0) in vec2 aVert;

vec2 vertices[] = {
    {-0.5, -0.5},
    {-0.5, 0.5},
    {0.5, 0.5}
};

void main() {
    gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
}