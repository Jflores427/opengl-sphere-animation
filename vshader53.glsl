/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source and other light sources;
  - Entire shading computation is done in the Eye Frame.
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec3 vNormal;
in  vec2 vTexCoord;

out vec4 eyePosition;
out vec4 color;

out vec2 texCoord;
out float texCoord1D;
out vec2 texCoord2D;

out vec2 latticeTexCoord;

// Shading/Lighting Flags
uniform bool IsAxesEnabled;
uniform bool IsPlaneEnabled;
uniform bool IsWireframeEnabled;
uniform bool IsShadowEnabled;
uniform bool IsLightingEnabled;
uniform bool IsFlatShadingEnabled;
uniform bool IsSmoothShadingEnabled;
uniform bool IsSpotlight;      // false => point source, true => spotlight

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 NormalMatrix;

uniform vec4 GlobalAmbient; 

uniform vec4 LightDirection;     // Directional light direction (w = 0.0) (passed in eye frame) [originally in eye]
uniform vec4 DirLightAmbient;
uniform vec4 DirLightDiffuse;
uniform vec4 DirLightSpecular;

uniform vec4 LightPosition;   // Positional light direction (w = 1.0) (passed in eye frame)
uniform vec4 LightAmbient;     
uniform vec4 LightDiffuse;
uniform vec4 LightSpecular;

uniform vec3 SpotlightDirection;    // Spot light direction (passed in eye frame)
uniform float SpotlightExponent;    // Spot exponent
uniform float SpotlightCutoff;      // Spot cutoff angle in radians
   
uniform vec4 MaterialAmbient;
uniform vec4 MaterialDiffuse;
uniform vec4 MaterialSpecular;

uniform float Shininess;

uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation

// Texture Mapping / Lattice Flags
uniform bool IsEyeSpace;
uniform int SphereMappingMode;

uniform bool IsLatticeOn;
uniform int LatticeMappingMode; // 0 = upright, 1 = tilted

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;

    vec3 N = normalize(NormalMatrix * vNormal);

    vec3 E = normalize(-pos); // Viewer eye vector

    if (dot(N, E) < 0) N = -N;

    if (IsLightingEnabled) {
        if (IsWireframeEnabled) {  // Wireframe mode (no lighting or shading)
            color = vec4(1.0, 0.84, 0.0, 1.0); // Wireframe color (yellow)
            gl_Position = Projection * ModelView * vPosition;
            return;
        }

        vec4 totalColor = GlobalAmbient * MaterialAmbient;

        // Directional Light 
        vec3 dir_L = normalize(-LightDirection.xyz);
        vec3 dir_H = normalize(dir_L + E);

        float dir_d = max(dot(N, dir_L), 0.0);
        vec4 dirDiffuse = dir_d * DirLightDiffuse * MaterialDiffuse;

        float dir_s = pow(max(dot(N, dir_H), 0.0), Shininess);
        vec4 dirSpecular = dir_s * DirLightSpecular * MaterialSpecular;

        if (dot(N, dir_L) < 0.0) dirSpecular = vec4(0.0);

        float attenuation = 1.0;

        totalColor += attenuation * (DirLightAmbient * MaterialAmbient + dirDiffuse + dirSpecular);

        // Positional Light
        vec3 pos_L = LightPosition.xyz - pos;
        float distance = length(pos_L);
        pos_L = normalize(pos_L);
        vec3 pos_H = normalize(pos_L + E);

        attenuation = 1.0 / (ConstAtt + LinearAtt * distance + QuadAtt * distance * distance);

        float pos_d = max(dot(N, pos_L), 0.0);
        vec4 posDiffuse = pos_d * LightDiffuse * MaterialDiffuse;

        float pos_s = pow(max(dot(N, pos_H), 0.0), Shininess);
        vec4 posSpecular = pos_s * LightSpecular * MaterialSpecular;

        if (dot(N, pos_L) < 0.0) posSpecular = vec4(0.0);

        // Spotlight
        vec4 spotlightAttenuation = vec4(1.0); // Starting value for spotEffect
        if (IsSpotlight) {
            float spotCos = dot(normalize(SpotlightDirection), -pos_L); // pos_L points from light to vert & SpotlightDirection is already in the eye frame
            if (spotCos < cos(SpotlightCutoff)) { // SpotlightCutOff is already in radians
                spotlightAttenuation = vec4(0.0); // outside spotlight cone
            }
            else {
                spotlightAttenuation = vec4(pow(spotCos, SpotlightExponent));
            }
        }

        vec4 posAmbient = LightAmbient * MaterialAmbient;
        totalColor += attenuation * spotlightAttenuation * (posAmbient + posDiffuse + posSpecular);

        color = totalColor;
    }
    else {
        if (IsAxesEnabled || IsPlaneEnabled) {
            color = MaterialDiffuse;
        }
        else if (IsShadowEnabled) {  // Shadow effect (if enabled)
            color = vec4(0.25, 0.25, 0.25, 0.65); // Dark shadow with transparency
        }
        else if (IsWireframeEnabled) {  // Wireframe mode (no lighting or shading)
            color = vec4(1.0, 0.84, 0.0, 1.0); // Wireframe color (yellow)
        }
        else {
            color = MaterialDiffuse;
        }
    }

    // Final transformation
    gl_Position = Projection * ModelView * vPosition;

    eyePosition = ModelView * vPosition;

    vec4 vert = IsEyeSpace ? ModelView * vPosition : vPosition;

    // 1-D Sphere Texture Coord Mapping
    if (SphereMappingMode == 0)       // Vertical
        texCoord1D = 2.5 * vert.x;
    else                       // Slanted
        texCoord1D = 1.5 * (vert.x + vert.y + vert.z);

    // 2-D Sphere Texture Coord Mapping
    if (SphereMappingMode == 0) {   // Vertical
        texCoord2D.x = 0.75 * (vert.x + 1.0);
        texCoord2D.y = 0.75 * (vert.y + 1.0);
    }
    else {  // Slanted
        texCoord2D.x = 0.45 * (vert.x + vert.y + vert.z);
        texCoord2D.y = 0.45 * (vert.x - vert.y + vert.z);
    }

    texCoord = vTexCoord;

    if (LatticeMappingMode == 0) { // Upright
        latticeTexCoord = vec2(0.5 * (vPosition.x + 1.0), 0.5 * (vPosition.y + 1.0));
    }
    else { // Tilted
        latticeTexCoord = vec2(0.3 * (vPosition.x + vPosition.y + vPosition.z), 0.3 * (vPosition.x - vPosition.y + vPosition.z));
    }
}