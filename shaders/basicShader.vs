#version 330 core

layout(location = 0) in vec3 position;       // Vertex position
layout(location = 1) in vec2 aTexCoord;      // Texture coordinates
// layout(location = 2) in mat4 modelMatrix;    // Per-instance model matrix (4 attributes)

uniform mat4 vp;  // View-Projection matrix

out vec2 TexCoord;

void main() {
    gl_Position = vp * vec4(position, 1.0); // Combine matrices for final position
    TexCoord = aTexCoord;
}
