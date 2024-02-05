#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;


uniform sampler2D TextureImage2;   // textura utilizada pelo REAPER


// Matrizes computadas no código C++ e enviadas para a GPU

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Parâmetros da axis-aligned bounding box (AABB) do modelo

uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923



out vec4 position_world;
out vec4 position_model;
out vec4 normal;


// Variaveis para implementar gouraud shading.

uniform vec4 light_position;
out vec4 cor_v;    // será a cor "recebida" pelo REAPER no fragment_shader


void main()
{

     // Coordenadas de textura

    float U = 0; 
    float V = 0;
    
        
    gl_Position = projection * view * model * model_coefficients; 

    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    position_model = model_coefficients;
    position_world = model * model_coefficients;
    vec4 camera_position = inverse(view) * origin;





     // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);      


    // Refletância especular

    vec3 Ks; 


    int q = 1;


    float minx = bbox_min.x;   
    float maxx = bbox_max.x;
    float miny = bbox_min.y;
    float maxy = bbox_max.y;


    U = (position_model.x - minx) / (maxx - minx);
    V = (position_model.y - miny) / (maxy - miny);


    vec3 Kd = texture(TextureImage2,vec2(U,V)).rgb;

    normal = inverse(transpose(model)) * normal_coefficients;  // Esse tem nos dois

    normal.w = 0.0;

    vec4 p = position_world;

    // Seguindo os slides 197 e 198 da aula de Mapeamento de Texturas.

    vec4 l = normalize(light_position - position_world);      
    vec4 n = normalize(normal);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    float lambert = max(0,dot(n,l));

    vec4 r = vec4(0.0,0.0,0.0,0.0); 
    r = 2 * n * dot(n,l) - l;


    // Espectro da luz ambiente

    vec3 Ia = vec3(0.2,0.2,0.2);
    
    vec3 Ka = vec3(0.3,0.3,0.3);

    vec3 ambient_term = vec3(0.0,0.0,0.0); 
    ambient_term = Ka * Ia;


    vec4 h = (v+l) / length(v+l);

    vec3 blinn_phong_specular_term  = vec3(0.0,0.0,0.0); 
    blinn_phong_specular_term = Ks * I * pow (max (0, dot(n,h)) , q);


  //  vec3 phong_specular_term  = vec3(0.0,0.0,0.0); 
  //  phong_specular_term = Ks * I * pow (max (0, dot(r,v)) , q);

    cor_v.a = 1;

    cor_v.rgb = Kd * I * lambert + ambient_term + blinn_phong_specular_term;
    cor_v.rgb = pow(cor_v.rgb, vec3(1.0,1.0,1.0)/2.2);

}

