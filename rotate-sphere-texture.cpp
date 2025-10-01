/************************************************************
 * File: rotate-sphere-texture.cpp (Code for Shader-Based OpenGL ---
                                 for OpenGL version 3.1 and later)

 * (The functions Rotate() and NormalMatrix() are added to the file "mat-yjc-new.h"
   by Yi-Jen Chiang, where a new and correct transpose function "transpose1()" and
   other related functions such as inverse(m) for the inverse of 3x3 matrix m are
   also added; see the file "mat-yjc-new.h". Modified by Yi-Jen Chiang to include the use of a general rotation function
   Rotate(angle, x, y, z), where the vector (x, y, z) can have length != 1.0,
   and also to include the use of the function NormalMatrix(mv) to return the
   normal matrix (mat3) of a given model-view matrix mv (mat4).)

 * Extensively modified by Yi-Jen Chiang for the program structure and user
   interactions. See the function keyboard() for the keyboard actions.
   Also extensively re-structured by Yi-Jen Chiang to create and use the new
   function drawObj() so that it is easier to draw multiple objects. Now a floor
   and a rotating cube are drawn.

** Perspective view of a color Sphere using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
   those colors across the triangles.
**************************************************************/

#include "Angel-yjc.h"
#include "texmap.c"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec3  point3;
 
GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);
void setupLightingUniformVars(mat4 mv);
mat4 computeShadowMatrix(vec4 light_position);

enum MenuOptions {
    MENU_RESET,
    MENU_QUIT,
    MENU_SHADOW_NO,
    MENU_SHADOW_YES,
    MENU_LIGHTING_NO,
    MENU_LIGHTING_YES,
    MENU_WIREFRAME,
    MENU_FLAT,
    MENU_SMOOTH,
    MENU_POINT_SOURCE,
    MENU_SPOT_LIGHT,
    MENU_NO_FOG,
    MENU_LINEAR_FOG,
    MENU_EXPONENTIAL_FOG,
    MENU_EXPONENTIAL_SQUARE_FOG,
    MENU_BLENDING_SHADOW_NO,
    MENU_BLENDING_SHADOW_YES,
    MENU_TEXTURE_MAPPED_GROUND_YES,
    MENU_TEXTURE_MAPPED_GROUND_NO,
    MENU_TEXTURE_MAPPED_SPHERE_YES_CL, // Contour Lines
    MENU_TEXTURE_MAPPED_SPHERE_YES_CB, // Checkerboard
    MENU_TEXTURE_MAPPED_SPHERE_NO,
    MENU_FIREWORKS_YES,
    MENU_FIREWORKS_NO,
};

GLuint program, fireworks_program;       /* shader program object id */
GLuint sphere_smooth_buffer, sphere_flat_buffer, plane_buffer, axes_buffer, fireworks_buffer; /* vertex buffer object ids for sphere, plane, axes, fireworks*/

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.1, zFar = 50.0;

// Sets eye for camera view
vec4 init_eye(7.0f, 3.0f, -10.0f, 1.0f); // initial viewer position for Sphere (VRP)
vec4 eye = init_eye;               // current viewer position

// Flags for the program
bool rolling_status = false;    // Rolling state (enabled/disabled)
bool animation_started = false;

int animation_flag = 0; // 1: animation; 0: non-animation. Toggled by key 'b' or 'B'

// Vertex shader color flags
int axes_flag = 1;
int wireframe_flag = 0;
int shadow_flag = 1;
int plane_flag = 1;

int lighting_flag = 0;
int flat_shading_flag = 1;
int smooth_shading_flag = 0;

int light_source_flag = 0;
int spot_light_flag = 0;

int fog_type = 0;

int blending_shadow_flag = 0;

int texture_mapped_ground_flag = 0;

int texture_mapped_sphere_flag = 0;
int sphere_mapping_mode_flag = 0;
int eye_space_flag = 0;

int lattice_mapping_mode_flag = 0;
int lattice_on_flag = 0;

int fireworks_flag = 0;

// Sphere Rolling / Translation 
point4 positions[] = { point4(-4.0f, 1.0f, 4.0f, 1.0f), point4(3.0f, 1.0f, -4.0f, 1.0f), point4(-3.0f, 1.0f, -3.0f, 1.0f) }; // Coordinates for A, B, C // This could remain as point3?? Not sure, Would need to fix position, direction and rotationAxis back to point3/vec3 as well.
int current_segment = 0; // Tracks the current segment being rolled in by the sphere.
GLfloat sphere_radius = 0.0f; // Found from the maximum distance of a vertex in the sphere file to the origin.
GLfloat rotation_angle = 0.00f;
GLfloat speed = 0.02f;  // Adjust speed to match rolling effect; 0.02f is ideal...
point4 position = positions[0];  // Start at point A
vec4 direction; // Direction of the sphere rolling
vec4 rotation_axis; // Rotation axis vector of the sphere roling
mat4 M = mat4(1.0f); // Accumulated matrix M for the rotation of the Sphere
mat4 R; // Rotation matrix of the Sphere

// Sphere vertices data for points and normals
vector<point4> sphere_points;
vector<vec3> sphere_smooth_normals;
vector<vec3> sphere_flat_normals;

// Sets up the axes' vertices points and colors with the corresponding data
const int axes_num_vertices = 6;

point4 axes_points[axes_num_vertices] = {
    point4(0.0f, 0.02f, 0.0f, 1.0f),
    point4(5.0f, 0.02f, 0.0f, 1.0f),  // X-axis
    point4(0.0f, 0.0f, 0.0f, 1.0f),
    point4(0.0f, 10.0f, 0.0f, 1.0f),    // Y-axis
    point4(0.0f, 0.02f, 0.0f, 1.0f),
    point4(0.0f, 0.02f, 8.0f, 1.0f) // Z-axis
};

// Disable Lighting when you draw it, Use dummy unit vectors since we are disabling lighting and it doesnt matter
vec3 axes_normals[axes_num_vertices] = {
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f)
};


// Sets up the plane's vertices points and colors with the corresponding data
const int plane_num_vertices = 6;

point4 plane_points[plane_num_vertices] = {
    point4(105.0f, 0.0f, 108.0f, 1.0f),
    point4(105.0f, 0.0f, -104.0f, 1.0f),
    point4(-105.0f, 0.0f, -104.0f, 1.0f),
    point4(-105.0f, 0.0f, -104.0f, 1.0f),
    point4(-105.0f, 0.0f, 108.0f, 1.0f),
    point4(105.0f, 0.0f, 108.0f, 1.0f)
};

vec3 plane_normals[plane_num_vertices] = {
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
};

vec2 plane_tex_coords[6] = {
  vec2(6.0f, 5.0f),  // for a
  vec2(6.0f, 0.0f),  // for b
  vec2(0.0f, 0.0f),  // for c

  vec2(0.0f, 0.0f),  // for c
  vec2(0.0f, 5.0f),  // for d
  vec2(6.0f, 5.0f),  // for a 
};

/*----- Shader Lighting Parameters -----*/
color4 global_ambient = color4(1.0f, 1.0f, 1.0f, 1.0f);

point4 init_light_position(-14.0f, 12.0f, -3.0f, 1.0f);

// In World frame.
// Needs to transform it to Eye Frame
// before sending it to the shader(s).
point4 light_position = init_light_position; // w = 1 for positional
color4 light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f);
color4 light_diffuse = color4(0.0f, 0.0f, 0.0f, 1.0f);
color4 light_specular = color4(0.0f, 0.0f, 0.0f, 1.0f);

point4 light_direction = point4(0.1f, 0.0f, -1.0f, 0.0f); // w = 0 for directional; Already in the Eye Frame
color4 dir_light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f);
color4 dir_light_diffuse = color4(0.8f, 0.8f, 0.8f, 1.0f);
color4 dir_light_specular = color4(0.2f, 0.2f, 0.2f, 1.0f);

vec4 material_ambient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 material_diffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 material_specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);

// Spotlight properties
point3 spotlight_direction = normalize(point3(-6.0f - light_position.x, 0.0f - light_position.y, -4.5f - light_position.z));
float spotlight_exponent = 15.0f;
float spotlight_cutoff_degrees = 20.0f;
float spotlight_cutoff_radians = double(spotlight_cutoff_degrees) * 2 * M_PI / 360;

// Attenuation factors
float const_att = 2.0f;
float linear_att = 0.01f;
float quad_att = 0.001f;

float  material_shininess = 0.0f;

mat3 normal_matrix;

mat4 N = computeShadowMatrix(init_light_position);

int previous_lighting_flag = lighting_flag;
int previous_wireframe_flag = wireframe_flag;
int previous_shadow_flag = shadow_flag;
int previous_texture_mapped_ground_flag = texture_mapped_ground_flag;
int previous_texture_mapped_sphere_flag = texture_mapped_sphere_flag;
int previous_lattice_on_flag = lattice_on_flag;

// Fog properties
vec4 fog_color = vec4(0.7f, 0.7f, 0.7f, 0.5f);
float fog_start = 0.0f;
float fog_end = 18.0f;
float fog_density = 0.09f;

// Texture program IDs
GLuint tex_1D;
GLuint tex_2D;

// Fireworks
const int fireworks_particle_count = 300;
vector<vec3> fireworks_colors;   // rgb format in xyz 
vector<vec3> fireworks_velocities; // vx, vy, vz in xyz

float t_start; // Fireworks animation start time
float t_now;
float t_max = 4.0f;

/*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
mat4 mv;
vec4 at(0.0f, 0.0f, 0.0f, 1.0f);
vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

// Projection Matrix
mat4 p;


//----------------------------------------------------------------------
// computeShadowMatrix(vec4 light_position):
// Set up shadow matrix based on light_position in world frame.
//----------------------------------------------------------------------
mat4 computeShadowMatrix(vec4 light_position) {
    return mat4(
        vec4(light_position.y, -light_position.x, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, -light_position.z, light_position.y, 0.0f),
        vec4(0.0f, -1.0f, 0.0f, light_position.y)
    );
}

//----------------------------------------------------------------------
// setupLightingUniformVars(mat4 mv):
// Set up lighting parameters that are uniform variables in shader.
//
// Note: "LightPosition" and "SpotlightDirection" in shader must be in the Eye Frame.
//       So we use parameter "mv", the model-view matrix, to transform
//       light_position and spotlight_direction to the Eye Frame.
//----------------------------------------------------------------------
void setupLightingUniformVars(mat4 mv)
{
    glUniform4fv(glGetUniformLocation(program, "GlobalAmbient"),
        1, global_ambient);

    // The Light Position needs to be in Eye Frame
    vec4 light_position_eyeFrame = mv * light_position;
    glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position_eyeFrame);

    glUniform4fv(glGetUniformLocation(program, "LightAmbient"),
        1, light_ambient);
    glUniform4fv(glGetUniformLocation(program, "LightDiffuse"),
        1, light_diffuse);
    glUniform4fv(glGetUniformLocation(program, "LightSpecular"),
        1, light_specular);

    // The Light Direction already in Eye Frame
    glUniform4fv(glGetUniformLocation(program, "LightDirection"), 1, light_direction); 
    glUniform4fv(glGetUniformLocation(program, "DirLightAmbient"), 1, dir_light_ambient);
    glUniform4fv(glGetUniformLocation(program, "DirLightDiffuse"), 1, dir_light_diffuse);
    glUniform4fv(glGetUniformLocation(program, "DirLightSpecular"), 1, dir_light_specular);

    glUniform4fv(glGetUniformLocation(program, "MaterialAmbient"),
        1, material_ambient);
    glUniform4fv(glGetUniformLocation(program, "MaterialDiffuse"),
        1, material_diffuse);
    glUniform4fv(glGetUniformLocation(program, "MaterialSpecular"),
        1, material_specular);

    glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"),
        1, GL_TRUE, normal_matrix);

    glUniform1i(glGetUniformLocation(program, "IsAxesEnabled"), axes_flag);
    glUniform1i(glGetUniformLocation(program, "IsPlaneEnabled"), plane_flag);
    glUniform1i(glGetUniformLocation(program, "IsWireframeEnabled"), wireframe_flag);
    glUniform1i(glGetUniformLocation(program, "IsShadowEnabled"), shadow_flag);
    glUniform1i(glGetUniformLocation(program, "IsLightingEnabled"), lighting_flag);
    glUniform1i(glGetUniformLocation(program, "IsFlatShadingEnabled"), flat_shading_flag);
    glUniform1i(glGetUniformLocation(program, "IsSmoothShadingEnabled"), smooth_shading_flag);
    glUniform1i(glGetUniformLocation(program, "IsSpotlight"), spot_light_flag);

    // The Spotlight (spotlight_direction) needs to be in Eye Frame
    vec3 spotlight_direction_eyeFrame = upperLeftMat3(mv) * spotlight_direction;
    glUniform3fv(glGetUniformLocation(program, "SpotlightDirection"),
        1, spotlight_direction_eyeFrame); // Convert to eye frame -> Normalize in vertex Shader
    glUniform1f(glGetUniformLocation(program, "SpotlightExponent"),
        spotlight_exponent);
    glUniform1f(glGetUniformLocation(program, "SpotlightCutoff"),
        spotlight_cutoff_radians);

    glUniform1f(glGetUniformLocation(program, "ConstAtt"),
        const_att);
    glUniform1f(glGetUniformLocation(program, "LinearAtt"),
        linear_att);
    glUniform1f(glGetUniformLocation(program, "QuadAtt"),
        quad_att);

    glUniform1f(glGetUniformLocation(program, "Shininess"),
        material_shininess);
}

//----------------------------------------------------------------------
// SetupTextureUniformVars():
// Set up texture parameters that are uniform variables in shader.
//
//----------------------------------------------------------------------
void setupTextureUniformVars()
{
    glUniform4fv(glGetUniformLocation(program, "FogColor"), 1, fog_color);
    glUniform1i(glGetUniformLocation(program, "FogType"), fog_type);
    glUniform1f(glGetUniformLocation(program, "FogStart"), fog_start);
    glUniform1f(glGetUniformLocation(program, "FogEnd"), fog_end);
    glUniform1f(glGetUniformLocation(program, "FogDensity"), fog_density);

    glUniform1i(glGetUniformLocation(program, "IsTextureMappedGround"), texture_mapped_ground_flag);

    glUniform1i(glGetUniformLocation(program, "IsEyeSpace"), eye_space_flag);
    glUniform1i(glGetUniformLocation(program, "TextureMappedSphereFlag"), texture_mapped_sphere_flag);
    glUniform1i(glGetUniformLocation(program, "SphereMappingMode"), sphere_mapping_mode_flag);

    glUniform1i(glGetUniformLocation(program, "IsLatticeOn"), lattice_on_flag);
    glUniform1i(glGetUniformLocation(program, "LatticeMappingMode"), lattice_mapping_mode_flag);
}

//----------------------------------------------------------------------------
// findRadius(): 
// Finds the max radius of a sphere file whose vertices' points have been inserted into sphere_points up until this point
// 
//----------------------------------------------------------------------------
void findRadius() {
    float max_radius = 0.0f;
    for (point4 sp : sphere_points) {
        max_radius = max(sqrt(sp.x * sp.x + sp.y * sp.y + sp.z * sp.z), max_radius);
    }
    sphere_radius = max_radius;
}

//----------------------------------------------------------------------------
// readSphereFile(fileName): 
// Reads a file in the appropriate vertex format to populate sphere_points and sphere_colors.
//
//----------------------------------------------------------------------------
void readSphereFile(const string fileName) 
{
        vector<point4> points;
        vector<vec3> smooth_normals;
        vector<vec3> flat_normals;
        ifstream file(fileName);
    
        if (!file) {
            cerr << "Error: Sphere file could not be opened: " << fileName << "\n";
            return;
        }
    
        // Triangle count from file
        int triangleCount;
        file >> triangleCount;
    
        // Initialize coordinate variables for each triangle
        float x, y, z;
        
        // Vertices in the Triangle (Should be 3)
        int n;
    
        for (int i = 0; i < triangleCount; i++) {
            file >> n;
    
            if (n != 3) {
                cerr << "Error: Expected triangle but found a different shape, with a vertex count of: " << n << "\n";
                continue;
            }

            point3 vertices[3];
            
            for (int j = 0; j < 3; j++) {
                file >> x >> y >> z;
                points.push_back(point4(x, y, z, 1.0f));
                
                // Compute per-vertex normal for smooth shading
                vec3 normal = normalize(vec3(x, y, z));
                smooth_normals.push_back(normal);

                vertices[j] = point3(x, y, z);
            }

            // Compute face normal for flat shading
            vec3 u = vec3(vertices[1]) - vec3(vertices[0]);
            vec3 v = vec3(vertices[2]) - vec3(vertices[0]);
            vec3 normal = normalize(cross(u, v));

            for (int j = 0; j < 3; j++) {
                flat_normals.push_back(normal);
            }

        }

        sphere_points = points;
        sphere_smooth_normals = smooth_normals;
        sphere_flat_normals = flat_normals;
        file.close();
}

//----------------------------------------------------------------------------
// populateFireworks(): 
// Populates the fireworks_velocities and fireworks_colors with random velocity/color values.
//
//----------------------------------------------------------------------------
void populateFireworks() {
    for (int i = 0; i < fireworks_particle_count; i++) {
        vec3 velocity;
        velocity.x = 2.0f * ((rand() % 256) / 256.0f - 0.5f);          // vx ∈ [-1,1]
        velocity.y = 1.2f * 2.0f * ((rand() % 256) / 256.0f);          // vy ∈ [0,2.4]
        velocity.z = 2.0f * ((rand() % 256) / 256.0f - 0.5f);          // vz ∈ [-1,1]
        fireworks_velocities.push_back(velocity);

        vec3 color;
        color.x = (rand() % 256) / 256.0f;          
        color.y = (rand() % 256) / 256.0f;      
        color.z = (rand() % 256) / 256.0f;          
        fireworks_colors.push_back(color);
    }
}

//----------------------------------------------------------------------------
// OpenGL initialization
//
void init()
{
    //readSphereFile("sphere.8.txt");    // Uncomment this line to read from "sphere.8.txt" file within the project directory.
    //readSphereFile("sphere.128.txt");  // Uncomment this line to read from "sphere.128.txt" file within the project directory.
    //readSphereFile("sphere.256.txt");  // Uncomment this line to read from "sphere.256.txt" file within the project directory.
    //readSphereFile("sphere.1024.txt"); // Uncomment this line to read from "sphere.1024.txt" file within the project directory.
    cout << "Enter Name of Sphere File (.txt extension) \n";
    string inputFile;
    cin >> inputFile;
    readSphereFile(inputFile);

    findRadius();

    populateFireworks();

    image_set_up();
    
    // Set up the textures
    glGenTextures(1, &tex_1D);
    glGenTextures(1, &tex_2D);

    glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_1D, tex_1D);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);

    // Set up texture wrapping and filtering
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1 
    glBindTexture(GL_TEXTURE_2D, tex_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

    // Set up texture wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Modulate mode for combining texture color with lighting/color
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Create and initialize a vertex buffer object for smooth shading sphere, to be used in display(), add the sphere_points and sphere_smooth_normals data to the buffer.
    glGenBuffers(1, &sphere_smooth_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_smooth_buffer);
    glBufferData(GL_ARRAY_BUFFER, sphere_points.size() * sizeof(point4) + sphere_smooth_normals.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere_points.size() * sizeof(point4), sphere_points.data()); 
    glBufferSubData(GL_ARRAY_BUFFER, sphere_points.size() * sizeof(point4), sphere_smooth_normals.size() * sizeof(vec3), sphere_smooth_normals.data());

    // Create and initialize a vertex buffer object for flat shading sphere, to be used in display(), add the sphere_points and sphere_flat_normals data to the buffer.
    glGenBuffers(1, &sphere_flat_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_flat_buffer);
    glBufferData(GL_ARRAY_BUFFER, sphere_points.size() * sizeof(point4) + sphere_flat_normals.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere_points.size() * sizeof(point4), sphere_points.data());
    glBufferSubData(GL_ARRAY_BUFFER, sphere_points.size() * sizeof(point4), sphere_flat_normals.size() * sizeof(vec3), sphere_flat_normals.data());

    // Create and initialize a vertex buffer object for axes, to be used in display(), add the axes_points and axes_colors data to the buffer.
    glGenBuffers(1, &axes_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, axes_buffer);
    glBufferData(GL_ARRAY_BUFFER, axes_num_vertices * sizeof(point4) + axes_num_vertices * sizeof(vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, axes_num_vertices * sizeof(point4), axes_points);
    glBufferSubData(GL_ARRAY_BUFFER, axes_num_vertices * sizeof(point4), axes_num_vertices * sizeof(vec3), axes_normals);

    // Create and initialize a vertex buffer object for plane, to be used in display(), add the plane_points and plane_colors data to the buffer.
    glGenBuffers(1, &plane_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, plane_buffer);
    glBufferData(GL_ARRAY_BUFFER, plane_num_vertices * sizeof(point4) + plane_num_vertices * sizeof(vec3) + plane_num_vertices * sizeof(vec2), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, plane_num_vertices * sizeof(point4), plane_points);
    glBufferSubData(GL_ARRAY_BUFFER, plane_num_vertices * sizeof(point4), plane_num_vertices * sizeof(vec3), plane_normals);
    glBufferSubData(GL_ARRAY_BUFFER, plane_num_vertices * sizeof(point4) + plane_num_vertices * sizeof(vec3), plane_num_vertices * sizeof(vec2), plane_tex_coords);

    // Create and initialize a vertex buffer object for fireworks, to be used in display(), add the fireworks_velocities and fireworks_colors data to the buffer.
    glGenBuffers(1, &fireworks_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, fireworks_buffer);
    glBufferData(GL_ARRAY_BUFFER, fireworks_particle_count * sizeof(vec3) + fireworks_particle_count * sizeof(vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, fireworks_velocities.size() * sizeof(vec3), fireworks_velocities.data());
    glBufferSubData(GL_ARRAY_BUFFER, fireworks_velocities.size() * sizeof(vec3), fireworks_colors.size() * sizeof(vec3), fireworks_colors.data());

    // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader53.glsl", "fshader53.glsl");
    fireworks_program = InitShader("fireworksVShader.glsl", "fireworksFShader.glsl");

    t_start = glutGet(GLUT_ELAPSED_TIME);

    glEnable( GL_DEPTH_TEST );

    // Sets background color to sky blue
    glClearColor(0.529, 0.807, 0.92, 0.0);
}

//----------------------------------------------------------------------------
// drawObj(buffer, offset, num_vertices, mode, line_width):
//   draw the object that is associated with the vertex buffer object "buffer"
//   having "num_vertices" vertices, a "mode" for the glDrawArrays() function, and a "line_width" 
//   for the same glDrawArrays() function.
//
//----------------------------------------------------------------------------
void drawObj(GLuint buffer, int offset, int num_vertices, GLenum mode, GLfloat line_width)
{
    glLineWidth(line_width);
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation(program, "vNormal"); 
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point4) * num_vertices) ); 

    /* Draw a sequence of geometric objs (triangles) from the vertex buffer
       (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(mode, offset, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vNormal);
    glLineWidth(1.0);
}

//----------------------------------------------------------------------------
// drawPlaneObj(buffer, offset, num_vertices, mode, line_width):
//   draw the plane object that is associated with the vertex buffer object "buffer"
//   having "num_vertices" vertices, a "mode" for the glDrawArrays() function, and a "line_width" 
//   for the same glDrawArrays() function.
//
//----------------------------------------------------------------------------
void drawPlaneObj(GLuint buffer, int offset, int num_vertices, GLenum mode, GLfloat line_width)
{
    glLineWidth(line_width);
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(point4) * num_vertices));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(point4) * num_vertices + sizeof(vec3) * num_vertices));
    // the offset is the (total) size of the previous vertex attribute array(s)

  /* Draw a sequence of geometric objs (triangles) from the vertex buffer
     (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(mode, offset, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vNormal);
    glDisableVertexAttribArray(vTexCoord);
    glLineWidth(1.0);
}

//----------------------------------------------------------------------------
// drawFireworksObj(buffer, offset, num_vertices, mode, point_size):
//   draw the fireworks object that is associated with the vertex buffer object "buffer"
//   having "num_vertices" vertices, a "mode" for the glDrawArrays() function, and a "point_size" 
//   for the same glDrawArrays() function.
//
//----------------------------------------------------------------------------
void drawFireworksObj(GLuint buffer, int offset, int num_vertices, GLenum mode, GLfloat point_size)
{
    glPointSize(point_size);
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vVelocity = glGetAttribLocation(fireworks_program, "vVelocity");
    glEnableVertexAttribArray(vVelocity);
    glVertexAttribPointer(vVelocity, 3, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(fireworks_program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
        BUFFER_OFFSET(sizeof(vec3) * num_vertices));
    // the offset is the (total) size of the previous vertex attribute array(s)

  /* Draw a sequence of geometric objs (triangles) from the vertex buffer
     (using the attributes specified in each enabled vertex attribute array) */
    glDrawArrays(mode, offset, num_vertices);

    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vVelocity);
    glDisableVertexAttribArray(vColor);
    glPointSize(1.0);
}

//----------------------------------------------------------------------------
// storeCurrentFlagStates(): 
// Stores the current shading/lighting/lattice/other flag states of the program 
// into previously defined previous state flags. 
// 
//----------------------------------------------------------------------------
void storeCurrentFlagStates() {
    previous_lighting_flag = lighting_flag;
    previous_wireframe_flag = wireframe_flag;
    previous_shadow_flag = shadow_flag;
    previous_texture_mapped_ground_flag = texture_mapped_ground_flag;
    previous_texture_mapped_sphere_flag = texture_mapped_sphere_flag;
    previous_lattice_on_flag = lattice_on_flag;
}

//----------------------------------------------------------------------------
// drawAxes(model_view): 
// Sets up the model_view matrix, the relevant flags, 
// the color attributes, and draws the axes.
// 
//----------------------------------------------------------------------------
void drawAxes(GLuint model_view) {
    axes_flag = 1;
    wireframe_flag = 0;
    lighting_flag = 0;
    texture_mapped_ground_flag = 0;
    texture_mapped_sphere_flag = 0;
    lattice_on_flag = 0;

    mv = LookAt(eye, at, up);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

    setupTextureUniformVars();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    material_diffuse = color4(1.0, 0.0, 0.0, 1.0);
    setupLightingUniformVars(mv);
    drawObj(axes_buffer, 0, 2, GL_LINES, 2.0);  // draw the x-axis

    material_diffuse = color4(1.0, 0.0, 1.0, 1.0);
    setupLightingUniformVars(mv);
    drawObj(axes_buffer, 2, 2, GL_LINES, 2.0);  // draw the y-axis

    material_diffuse = color4(0.0, 0.0, 1.0, 1.0);
    setupLightingUniformVars(mv);
    drawObj(axes_buffer, 4, 2, GL_LINES, 2.0);  // draw the z-axis

    axes_flag = 0;
    lighting_flag = previous_lighting_flag;
    wireframe_flag = previous_wireframe_flag;
    shadow_flag = previous_shadow_flag;
    texture_mapped_ground_flag = previous_texture_mapped_ground_flag;
    texture_mapped_sphere_flag = previous_texture_mapped_sphere_flag;
    lattice_on_flag = previous_lattice_on_flag;
}

//----------------------------------------------------------------------------
// drawPlane(model_view): 
// Sets up the model_view and the normal_matrix matrices, the relevant flags, 
// the relevant texture mapping (if applicable), and draws the plane.
// 
//----------------------------------------------------------------------------
void drawPlane(GLuint model_view) {
    plane_flag = 1;
    wireframe_flag = 0;
    shadow_flag = 0;
    texture_mapped_sphere_flag = 0;
    lattice_on_flag = 0;

    mv = LookAt(eye, at, up);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    
    if (lighting_flag == 1) {
        material_ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
        material_diffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        material_specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        material_shininess = 0.0f;

        // Normal Matrix Calculation
        normal_matrix = NormalMatrix(mv, 1);

        if (light_source_flag == 1) {
            light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f);  // Positional Light Set
            light_diffuse = color4(1.0f, 1.0f, 1.0f, 1.0f);
            light_specular = color4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f); // Positional Light Removed
            light_diffuse = color4(0.0f, 0.0f, 0.0f, 1.0f);
            light_specular = color4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else {
        material_diffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    setupLightingUniformVars(mv);  // mv needs to be LookAt

    if (texture_mapped_ground_flag == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_2D);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    setupTextureUniformVars();

    if (plane_flag == 1) // Filled floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawPlaneObj(plane_buffer, 0, plane_num_vertices, GL_TRIANGLES, 1.0);  // draw the plane

    plane_flag = 0;
    wireframe_flag = previous_wireframe_flag;
    shadow_flag = previous_shadow_flag;
    texture_mapped_sphere_flag = previous_texture_mapped_sphere_flag;
    lattice_on_flag = previous_lattice_on_flag;
}

//----------------------------------------------------------------------------
// drawShadow(model_view): 
// Sets up the model_view and the normal_matrix matrices, the relevant flags, 
// the relevant blending attributes (if applicable), and draws the shadow.
// 
//----------------------------------------------------------------------------
void drawShadow(GLuint model_view) {
    lighting_flag = 0;
    texture_mapped_ground_flag = 0;
    texture_mapped_sphere_flag = 0;

    if (blending_shadow_flag == 1) {
        glUniform1i(glGetUniformLocation(program, "IsBlendingShadowEnabled"), blending_shadow_flag);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if (shadow_flag == 1 && eye.y > 0.0f) {
        mv = LookAt(eye, at, up) * N * Translate(position.x, position.y, position.z) * (rolling_status ? R : 1) * M;

        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

        // Set up the Normal Matrix from the model-view matrix
        normal_matrix = NormalMatrix(mv, 1);
        glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"),
            1, GL_TRUE, normal_matrix);

        if (wireframe_flag != 1) // Filled sphere
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else              // Wireframe sphere
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        setupLightingUniformVars(mv);  // mv needs to be LookAt

        setupTextureUniformVars();

        if (smooth_shading_flag == 1)
            drawObj(sphere_smooth_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the smooth sphere
        else if (flat_shading_flag == 1)
            drawObj(sphere_flat_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the flat sphere
        else
            drawObj(sphere_smooth_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the smooth sphere (By Default...) 
    }

    if (blending_shadow_flag == 1) {
        glUniform1i(glGetUniformLocation(program, "IsBlendingShadowEnabled"), 0);
        glDisable(GL_BLEND);
    }

    lighting_flag = previous_lighting_flag;
    texture_mapped_ground_flag = previous_texture_mapped_ground_flag;
    texture_mapped_sphere_flag = previous_texture_mapped_sphere_flag;
}

//----------------------------------------------------------------------------
// drawSphere(model_view): 
// Sets up the model_view and the normal_matrix matrices, the relevant flags, 
// the relevant texture (if applicable), and draws the sphere.
// 
//----------------------------------------------------------------------------
void drawSphere(GLuint model_view) {
    shadow_flag = 0;
    texture_mapped_ground_flag = 0;

    mv = LookAt(eye, at, up) * Translate(position.x, position.y, position.z) * (rolling_status ? R : 1) * M;

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

    if (lighting_flag == 1) {
        material_ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
        material_diffuse = vec4(1.0f, 0.84f, 0.0f, 1.0f);
        material_specular = vec4(1.0f, 0.84f, 0.0f, 1.0f);
        material_shininess = 125.0f;

        // Normal Matrix Calculation
        normal_matrix = NormalMatrix(mv, 1);

        if (light_source_flag == 1) {
            light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f);  // Positional Light Set
            light_diffuse = color4(1.0f, 1.0f, 1.0f, 1.0f);
            light_specular = color4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            light_ambient = color4(0.0f, 0.0f, 0.0f, 1.0f); // Positional Light Removed
            light_diffuse = color4(0.0f, 0.0f, 0.0f, 1.0f);
            light_specular = color4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else {
        material_diffuse = vec4(1.0, 0.84, 0.0, 1.0); // Wireframe color (yellow)
    }
    setupLightingUniformVars(LookAt(eye, at, up));  // Lighting model_view is irrelevant to the model_view transformation of the sphere...

    if (texture_mapped_sphere_flag == 1) {
        glEnable(GL_TEXTURE_1D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, tex_1D);
    }
    else if (texture_mapped_sphere_flag == 2) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_2D);
    }
    else {
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
    }

    setupTextureUniformVars();

    if (wireframe_flag != 1) // Filled sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe sphere
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (smooth_shading_flag == 1)
        drawObj(sphere_smooth_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the smooth sphere
    else if (flat_shading_flag == 1)
        drawObj(sphere_flat_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the flat sphere
    else
        drawObj(sphere_smooth_buffer, 0, sphere_points.size(), GL_TRIANGLES, 1.0);  // draw the smooth sphere (By Default...) 

    shadow_flag = previous_shadow_flag;
    texture_mapped_ground_flag = previous_texture_mapped_ground_flag;
}


//----------------------------------------------------------------------------
// drawFireworks(): 
// Sets up the model_view and projection matrices, the time stage for the particles,
// and draws the fireworks.
// 
//----------------------------------------------------------------------------
void drawFireworks() {
    glUseProgram(fireworks_program);

    GLuint fireworks_model_view = glGetUniformLocation(fireworks_program, "ModelView");
    GLuint fireworks_projection = glGetUniformLocation(fireworks_program, "Projection");

    glUniformMatrix4fv(fireworks_projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
    glUniformMatrix4fv(fireworks_model_view, 1, GL_TRUE, LookAt(eye, at, up)); // GL_TRUE: matrix is row-major

    /* -- Fireworks particle time setting -- */
    t_now = glutGet(GLUT_ELAPSED_TIME);
    float t = 0.001f * (t_now - t_start);
    if (t > t_max) t_start = t_now;

    glUniform3fv(glGetUniformLocation(fireworks_program, "StartPos"), 1, vec3(0.0f, 0.1f, 0.0f));
    glUniform1f(glGetUniformLocation(fireworks_program, "CurrentTime"), t);

    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    drawFireworksObj(fireworks_buffer, 0, fireworks_particle_count, GL_POINTS, 3.0f);
}

//----------------------------------------------------------------------------
// display(void): 
// Sets up the model_view and projection matrices for the sphere, plane, shadow, 
// and axes and draws the corresponding objects.
// 
//----------------------------------------------------------------------------
void display(void)
{
    GLuint  model_view;  // model-view matrix uniform shader variable location
    GLuint  projection;  // projection matrix uniform shader variable location

    GLuint  fireworks_model_view;  // model-view matrix uniform shader variable location
    GLuint  fireworks_projection;  // projection matrix uniform shader variable location

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "ModelView" );  
    projection = glGetUniformLocation(program, "Projection" );
    
    glUniform1i(glGetUniformLocation(program, "Texture_1D"), 0);  // Texture unit 0
    glUniform1i(glGetUniformLocation(program, "Texture_2D"), 1);  // Texture unit 1

    /*---  Set up and pass on Projection matrix to the shader ---*/
    p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    /*---  Set up and pass on ViewMatrix to the shader ---*/
    mv = LookAt(eye, at, up);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

    /*--- Set up the Rotation Matrix for the Sphere ---*/
    R = Rotate(rotation_angle, rotation_axis.x, rotation_axis.y, rotation_axis.z);

    // Store the current flag states of the program
    storeCurrentFlagStates();

    /*----- Set up and draw the axes -----*/
    drawAxes(model_view);

    /* ------------------------------------------------------------- */
    // Critical Section: Start  (Updated for HW 4; Blended Shadow)
    // 1. Disable Writing to  Z-Buffer
    glDepthMask(GL_FALSE);

    /*----- Set up and draw the plane -----*/
    drawPlane(model_view);
    /* ----------------------------------------------------------*/

    /*----- Set up and draw the shadow rendering -----*/
    drawShadow(model_view);
    /* ----------------------------------------------------------*/

    // 2. Enable Writing to Z-Buffer
    glDepthMask(GL_TRUE);

    // 3. Disable Writing to Frame Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    /*----- Set up and draw the plane -----*/
    drawPlane(model_view);
    /* ----------------------------------------------------------*/

    // 4. Enable Writing to Frame Buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    // Critical Section: End
    /* ------------------------------------------------------------- */
    
    /*----- Set up the drawing for the sphere -----*/
    drawSphere(model_view);

    /*----- Set up and draw the fireworks, if enabled -----*/
    if (fireworks_flag == 1) {
        drawFireworks();
    }

    glutSwapBuffers();
}


//----------------------------------------------------------------------------
// setNewDirection(): Recalculate direction and rotation axis for sphere
//
//----------------------------------------------------------------------------
void setNewDirection() {
    vec4 nextPosition = positions[(current_segment + 1) % 3];
    direction = normalize(nextPosition - position); // Compute unit direction

    // Compute the rotation axis [cross product with y-axis]
    vec3 up(0, 1, 0);
    rotation_axis = cross(up, direction);
}

//----------------------------------------------------------------------------
// idle(): 
// Updates the position and rotation angle of the sphere, computes rotation matrix R, 
// updates M based on R, and invokes new direction when reaching the end of the rolling segment.
//
//----------------------------------------------------------------------------
void idle(void) {
    float distance = speed;
    position += distance * direction;  // Move sphere along path
    rotation_angle += (GLfloat) ((distance / (2 * M_PI * sphere_radius)) * 360.0);
    if (rotation_angle > 4.0f) rotation_angle = 0.0f; // Limits rotation angle to slow down the speed of rotation on the sphere
    
    mat4 R = Rotate(rotation_angle, rotation_axis.x, rotation_axis.y, rotation_axis.z); // Create R Matrix and update M with it.
    M = R * M;

    // Check if the sphere reaches the next point
    vec4 nextPosition = positions[(current_segment + 1) % 3];

    if (length((position - nextPosition)) < 0.05f) {
        position = nextPosition;  // Goes to next position
        current_segment = (current_segment + 1) % 3;  // Move to next rolling segment
        setNewDirection(); 
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------
// menu(option): 
// Provides the menu callback for the program
//
//----------------------------------------------------------------------------
void menu(int option) {
    switch (option) {
        // Resets the view
        case MENU_RESET: 
            eye.x = 7.0f; eye.y = 3.0f; eye.z = -10.0f;
            break;
        case MENU_QUIT:
            exit(0);
            break;
        case MENU_WIREFRAME:
            wireframe_flag = !wireframe_flag;
            break;
        case MENU_SHADOW_NO:
            shadow_flag = 0;
            break;
        case MENU_SHADOW_YES:
            shadow_flag = 1;
            break;
        case MENU_LIGHTING_NO:
            lighting_flag = 0;
            light_source_flag = 0;
            break;
        case MENU_LIGHTING_YES:
            lighting_flag = 1;
            break;
        case MENU_SMOOTH:
            flat_shading_flag = 0;
            smooth_shading_flag = 1;
            break;
        case MENU_FLAT:
            flat_shading_flag = 1;
            smooth_shading_flag = 0;
            break;
        case MENU_SPOT_LIGHT:
            lighting_flag = 1;
            light_source_flag = 1;
            spot_light_flag = 1;
            break;
        case MENU_POINT_SOURCE:
            lighting_flag = 1;
            light_source_flag = 1;
            spot_light_flag = 0;
            break; 
        case MENU_NO_FOG:
            fog_type = 0;
            break;
        case MENU_LINEAR_FOG:
            fog_type = 1;
            break;
        case MENU_EXPONENTIAL_FOG:
            fog_type = 2;
            break;
        case MENU_EXPONENTIAL_SQUARE_FOG:
            fog_type = 3;
            break;
        case MENU_BLENDING_SHADOW_YES:
            blending_shadow_flag = 1;
            break;
        case MENU_BLENDING_SHADOW_NO:
            blending_shadow_flag = 0;
            break;
        case MENU_TEXTURE_MAPPED_GROUND_YES:
            texture_mapped_ground_flag = 1;
            break;
        case MENU_TEXTURE_MAPPED_GROUND_NO:
            texture_mapped_ground_flag = 0;
            break;
        case MENU_TEXTURE_MAPPED_SPHERE_YES_CL:
            texture_mapped_sphere_flag = 1;
            break;
        case MENU_TEXTURE_MAPPED_SPHERE_YES_CB:
            texture_mapped_sphere_flag = 2;
            break;
        case MENU_TEXTURE_MAPPED_SPHERE_NO:
            texture_mapped_sphere_flag = 0;
            break;
        case MENU_FIREWORKS_YES:
            if (fireworks_flag == 0) t_start = glutGet(GLUT_ELAPSED_TIME);
            fireworks_flag = 1;
            break;
        case MENU_FIREWORKS_NO:
            fireworks_flag = 0;
            break;
        }
        glutPostRedisplay();
}

//----------------------------------------------------------------------------
// createMenu(): 
// Creates the menu interface with the respective entries associated with the menu callback
//
//----------------------------------------------------------------------------
void createMenu() {

    int shadow_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(shadow_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_SHADOW_NO);
    glutAddMenuEntry(" Yes ", MENU_SHADOW_YES);

    int lighting_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(lighting_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_LIGHTING_NO);
    glutAddMenuEntry(" Yes ", MENU_LIGHTING_YES);

    int shading_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(shading_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Flat Shading ", MENU_FLAT);
    glutAddMenuEntry(" Smooth Shading ", MENU_SMOOTH);

    int light_source_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(light_source_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Spot Light ", MENU_SPOT_LIGHT);
    glutAddMenuEntry(" Point Source ", MENU_POINT_SOURCE);

    int fog_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(fog_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No Fog ", MENU_NO_FOG);
    glutAddMenuEntry(" Linear ", MENU_LINEAR_FOG);
    glutAddMenuEntry(" Exponential ", MENU_EXPONENTIAL_FOG);
    glutAddMenuEntry(" Exponential Square ", MENU_EXPONENTIAL_SQUARE_FOG);

    int blending_shadow_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(blending_shadow_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_BLENDING_SHADOW_NO);
    glutAddMenuEntry(" Yes ", MENU_BLENDING_SHADOW_YES);

    int textured_mapped_ground_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(textured_mapped_ground_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_TEXTURE_MAPPED_GROUND_NO);
    glutAddMenuEntry(" Yes ", MENU_TEXTURE_MAPPED_GROUND_YES);

    int textured_mapped_sphere_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(textured_mapped_sphere_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_TEXTURE_MAPPED_SPHERE_NO);
    glutAddMenuEntry(" Yes - Contour Lines ", MENU_TEXTURE_MAPPED_SPHERE_YES_CL);
    glutAddMenuEntry(" Yes - Checkerboard ", MENU_TEXTURE_MAPPED_SPHERE_YES_CB);

    int fireworks_menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(fireworks_menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" No ", MENU_FIREWORKS_NO);
    glutAddMenuEntry(" Yes ", MENU_FIREWORKS_YES);

    int menu_ID = glutCreateMenu(menu);
    glutSetMenuFont(menu_ID, GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry(" Default View Point ", 0);
    glutAddMenuEntry(" Wire Frame Sphere ", MENU_WIREFRAME);
    glutAddSubMenu(" Shadow ", shadow_menu_ID);
    glutAddSubMenu(" Enable Lighting ", lighting_menu_ID);
    glutAddSubMenu(" Shading ", shading_menu_ID);
    glutAddSubMenu(" Light Source ", light_source_menu_ID);
    glutAddSubMenu(" Fog Options ", fog_menu_ID);
    glutAddSubMenu(" Blending Shadow ", blending_shadow_menu_ID);
    glutAddSubMenu(" Texture Mapped Ground ", textured_mapped_ground_menu_ID);
    glutAddSubMenu(" Texture Mapped Sphere ", textured_mapped_sphere_menu_ID);
    glutAddSubMenu(" Firework ", fireworks_menu_ID);
    glutAddMenuEntry(" Quit ", MENU_QUIT);
    glutAttachMenu(GLUT_LEFT_BUTTON);
}


//----------------------------------------------------------------------------
// mouse(button, state, x, y): 
// Provides the mouse callback with the respective arguments for the program based on mouse click events
//
//----------------------------------------------------------------------------
void mouse(int button, int state, int x, int y)
{
    if (animation_started && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        rolling_status = rolling_status ? false : true;
        animation_flag = 1 - animation_flag;
        if (animation_started && animation_flag == 1) glutIdleFunc(idle);
        else if (animation_started && animation_flag == 0) glutIdleFunc(NULL);
    }

    glutPostRedisplay();
}


//----------------------------------------------------------------------------
// keyboard(key, x, y): 
// Provides the keyboard callback with the respective arguments for the program based on keyboard input events
//
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
	    case 033: // Escape Key
	    case 'q': case 'Q':
	        exit( EXIT_SUCCESS );
	        break;

        case 'X': eye[0] += 1.0; break;
	    case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
	    case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
	    case 'z': eye[2] -= 1.0; break;
        case 'b': 
        case 'B': // Toggle between animation and non-animation
            if(!animation_started) animation_started = true;
            rolling_status = rolling_status ? false : true;
	        animation_flag = 1 -  animation_flag;
            if (animation_flag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
            break;
	    case ' ':  // reset to initial viewer/eye position
	        eye = init_eye;
	        break;
        case 'o':
        case 'O':
            eye_space_flag = 0;
            break;
        case 'e':
        case 'E':
            eye_space_flag = 1;
            break;
        case 'v':
        case 'V':
            sphere_mapping_mode_flag = 0;
            break;
        case 's':
        case 'S':
            sphere_mapping_mode_flag = 1;
            break;
        case 'l': 
        case 'L':
            lattice_on_flag = !lattice_on_flag;
            break;
        case 'u': 
        case 'U':
            lattice_mapping_mode_flag = 0;
            break;
        case 't': 
        case 'T':
            lattice_mapping_mode_flag = 1;
            break;
    }
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
// reshape(width, height): 
// Provides the reshape callback with the respective arguments for the program based on window resizing
//
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}

//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    //glutCreateWindow("Color Cube");
    glutCreateWindow("Sphere Animation");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    { 
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
        exit(1);
    }
#endif

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    createMenu();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(NULL);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    init();

    setNewDirection();  // Initialize the first movement direction

    glutMainLoop();
    return 0;
}
