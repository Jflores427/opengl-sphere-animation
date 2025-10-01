/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 eyePosition;
in  vec4 color;

in  vec2 texCoord;
in  float texCoord1D;
in  vec2 texCoord2D;

in vec2 latticeTexCoord;

uniform vec4 FogColor; 

uniform int FogType;   // 0: no fog, 1: linear, 2: exp, 3: exp^2
uniform float FogStart;    // 0.0 for linear
uniform float FogEnd;      // 18.0 for linear
uniform float FogDensity;  // 0.09 for exp & exp^2

uniform bool IsBlendingShadowEnabled;

uniform sampler1D Texture_1D;
uniform sampler2D Texture_2D; 

uniform int IsTextureMappedGround; // 0: no texture application: obj color
                                    // 1: (obj color) * (texture color)

uniform int TextureMappedSphereFlag;

uniform bool IsWireframeEnabled;

uniform bool IsLatticeOn;

out vec4 fColor;

void main() 
{   
    if (!IsWireframeEnabled && IsLatticeOn) {
        float s = fract(4.0 * latticeTexCoord.s);
        float t = fract(4.0 * latticeTexCoord.t);
        if (s < 0.35 && t < 0.35)
            discard;
    }

    float fogFactor = 1.0;
    float z = -eyePosition.z;

    vec4 currColor = color;

    if (IsBlendingShadowEnabled)
        currColor = vec4(0.25f, 0.25f, 0.25f, 0.65f);  // Shadow Blending Color
    

    if (IsTextureMappedGround == 0)
        currColor = currColor;
    else if (IsTextureMappedGround == 1)
        currColor = currColor * texture(Texture_2D, texCoord);

    if (!IsWireframeEnabled) {
        if (TextureMappedSphereFlag == 0) {
            currColor = currColor;
        }
        else if (TextureMappedSphereFlag == 1) {
            vec4 texColor = texture(Texture_1D, texCoord1D);
            currColor = currColor * texColor * vec4(1.0f, 1.0f, 0.0f, 1.0f); // modulate with yellow
        }
        else if (TextureMappedSphereFlag == 2) {
            vec4 texColor = texture(Texture_2D, texCoord2D);

            // Change greenish into reddish, if found
            if (texColor.x < 0.2f && texColor.y > 0.5f && texColor.z < 0.2f)
                texColor = vec4(0.9f, 0.1f, 0.1f, 1.0f);

            currColor = currColor * texColor * vec4(1.0f, 1.0f, 0.0f, 1.0f);
        }
    }

    if (FogType == 1)   // Linear
        fogFactor = (FogEnd - z) / (FogEnd - FogStart);
    else if (FogType == 2)  // Exponential
        fogFactor = exp(-FogDensity * z);
    else if (FogType == 3)  // Exponential Squared
        fogFactor = exp(-pow(FogDensity * z, 2.0));

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalRGB = mix(FogColor.rgb, currColor.rgb, fogFactor);

    if (IsBlendingShadowEnabled) 
        fColor = vec4(finalRGB, 0.65f);
    else 
        fColor = vec4(finalRGB, fogFactor);
} 

