/*
File Name: "fireworksVShader.glsl":
Firework Particles Vertex Shader:
*/

#version 150

in vec3 vColor;
in vec3 vVelocity;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec3 StartPos;
uniform float CurrentTime; 

out float worldYPos;
out vec3 color;

void main() {
    float a = -0.00000049;
    float t = CurrentTime; // In seconds

    // Particle position
    vec4 position;
    position.x = StartPos.x + vVelocity.x * t;
    position.y = StartPos.y + vVelocity.y * t + 0.5 * a * t * t;
    position.z = StartPos.z + vVelocity.z * t;
    position.w = 1.0;

    color = vColor;
    worldYPos = position.y;

    gl_Position = Projection * ModelView * position;
}