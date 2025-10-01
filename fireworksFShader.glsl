/*
File Name: "fireworksFShader.glsl":
           Firework Particles Fragment Shader
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in vec3 color;
in float worldYPos;

out vec4 fColor;

void main () {
    // Discard Firework particle if its world position goes below y = 0.1
    if (worldYPos < 0.1) discard;

    fColor = vec4(color, 1.0);
}
