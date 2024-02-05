#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 position_model;


// Identificador que define qual objeto está sendo desenhado no momento

#define DOOR 0
#define STONE_EYES  1
#define STONE_HANDS_LEGS  2
#define STONE_HEAD  3
#define STONE_TORSO  4
#define REAPER 5
#define SCORPION 6
#define WALL_CUBE 7
#define FLOOR_CUBE 8
#define FENCEA 9
#define FENCEB 10
#define BULLETA 11
#define BULLETB 12
#define BULLETC 13
#define FAKE_CUBE 14


uniform int object_id;


// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;


// No caso do Reaper, em que será usado modelo de Gouraud, é preciso também
// definir a entrada ("in") vinda do vertex shader, segundo os slides 197 e 198
// das aulas 17 e 18 - Modelos de iluminação

in vec4 cor_v;



// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// Funções

float define_U_coeff (int projection_type);
float define_V_coeff (int projection_type);

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));  // aqui era 0.5 no inicial

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);


    // Caso queiramos que a fonte de luz seja a câmera, basta descomentar a linha abaixo
     // sentido da fonte de luz = sentido da cÂmera!

     l = v;    // tarefa 2.1 lab 04

    // Vetor que define o sentido da reflexão especular ideal.   // ch
    vec4 r = vec4(0.0,0.0,0.0,0.0); 
    r = 2 * n * dot(n,l) - l;

    // Parâmetros que definem as propriedades espectrais da superfície
   
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    // Coordenadas de textura

    float U = 0; 
    float V = 0;

    if ( object_id == STONE_EYES )          
    {
                                    // Olhos serão especulares 
        Kd = vec3(1.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.5,0.0,0.0);
        q = 1.0;
    }
    else if ( object_id == STONE_HANDS_LEGS )   // ch
    {
        Kd = vec3(0.8,0.4,0.08);  // Superfície 100% difusa, com refletância no modelo RGB = (0.8, 0.4, 0.08)
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.4,0.2,0.04);
        q = 1.0;
    }
    else if ( object_id == STONE_HEAD )   // ch
    {
        Kd = vec3(0.8,0.4,0.08); 
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.4,0.2,0.04);
        q = 1.0;
    }
    else if ( object_id == STONE_TORSO )   // ch
    {
        Kd = vec3(0.0,0.0,0.0); 
        Ks = vec3(1.0,1.0,1.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    else if ( object_id == REAPER )   
    {
        U = define_U_coeff(0);
        V = define_V_coeff(0);     
    }

    else if ( object_id == SCORPION )   
    {
        U = define_U_coeff(0);
        V = define_V_coeff(0); 
    }

     else if (object_id == FENCEA) 
    {
        U = define_U_coeff(1);
        V = define_V_coeff(1); 
    }

     else if (object_id == FENCEB) 
    {
        U = define_U_coeff(1);
        V = define_V_coeff(1); 
    }

     else if (object_id == BULLETA) 
    {
        U = define_U_coeff(0);
        V = define_V_coeff(0); 
    }

     else if (object_id == BULLETB) 
    {
        U = define_U_coeff(0);
        V = define_V_coeff(0); 
    }

    
     else if (object_id == BULLETC) 
    {
        U = define_U_coeff(0);
        V = define_V_coeff(0); 
    }


    else if (object_id == WALL_CUBE) 
    {
        U = define_U_coeff(1);
        V = define_V_coeff(1);                   
    }

    else if (object_id == FAKE_CUBE) 
    {
        U = define_U_coeff(1);
        V = define_V_coeff(1);                   
    }


     else if (object_id == FLOOR_CUBE) 
    {                                 
        U = define_U_coeff(1);
        V = define_V_coeff(1);
    }

    else        // Objeto desconhecido = verde  
    {
        Kd = vec3(0.0,0.6,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }


    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);           

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);        

    // Termo difuso utilizando a lei dos cossenos de Lambert

    vec3 lambert_diffuse_term = vec3(0.0,0.0,0.0); 
    lambert_diffuse_term = Kd * I * max (0, dot(n,l));

    // Termo ambiente

    vec3 ambient_term = vec3(0.1,0.1,0.1); 
    ambient_term = Ka * Ia;

   
    // Termo especular utilizando o modelo de iluminação de Blinn-Phong

    q = 0.5;
    Ks = vec3(0.01,0.01,0.01);

    vec4 h = (v+l) / length(v+l);

    vec3 blinn_phong_specular_term  = vec3(0.0,0.0,0.0); 
    blinn_phong_specular_term = Ks * I * pow (max (0, dot(n,h)) , q);

   // vec3 phong_specular_term  = vec3(0.0,0.0,0.0); 
  //  phong_specular_term = Ks * I * pow (max (0, dot(r,v)) , q);


    // Equação de Iluminação de lambert (uma das partes do lambert_diffuse_term)

    float lambert = max(0,dot(n,l));  
  
    if (object_id == WALL_CUBE) {     // Obtemos a refletância difusa a partir da leitura da imagem TextureImage0 , e assim por diante

       vec3 Kd = texture(TextureImage0, vec2(U,V)).rgb;
       color.rgb = Kd * I * lambert + ambient_term + blinn_phong_specular_term;

    }

    else if (object_id == FLOOR_CUBE) {

       vec3 Kd = texture(TextureImage1, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);

    }

     else if ( object_id == REAPER )
    {
        // Todas as caracteristicas de iluminação do REAPER
        // foram definidas no vertex shader

        color = cor_v;
    }

    else if (object_id == REAPER) {
                          
       vec3 Kd = texture(TextureImage2, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);
    }
    
    else if (object_id == SCORPION) {
  
       vec3 Kd = texture(TextureImage3, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);
    }

    else if (object_id == FENCEA || object_id == FENCEB ) {

       vec3 Kd = texture(TextureImage4, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);
    }

    else if (object_id == BULLETA || object_id == BULLETB || object_id == BULLETC) 
    {
       vec3 Kd = texture(TextureImage5, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);
    }

    else if (object_id == FAKE_CUBE) 
    {
       vec3 Kd = texture(TextureImage6, vec2(U,V)).rgb;
       color.rgb = Kd * (lambert + 0.01);
    }


    else {


      // Cor final do fragmento calculada com uma combinação dos termos difuso,
      // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.

      color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

    }


     color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);

} 


 float define_U_coeff (int projection_type) {


        float U = 0;

        if (projection_type == 0) {   // Projeção planar

           float minx = bbox_min.x;   
           float maxx = bbox_max.x;

          // float minz = bbox_min.z;
          //  float maxz = bbox_max.z;

                                                            // seguindo o slide 6
                                                            // do link https://moodle.inf.ufrgs.br/pluginfile.php/199727/mod_resource/content/2/Aula_22_Laboratorio_5.pdf
        U = (position_model.x - minx) / (maxx - minx);

        }

        else {
                                         // Projeção Esférica
             vec4 p_vector;
             vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
 
             p_vector = position_model - bbox_center;
             float symbol_theta = atan(p_vector.x,p_vector.z);

             U = (symbol_theta + M_PI) / (2 * M_PI);
    
        }
        return U;

 }


 float define_V_coeff (int projection_type) {

        float V = 0;

        if (projection_type == 0) {   // Projeção planar

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

       // float minz = bbox_min.z;
       // float maxz = bbox_max.z;

                                                            // seguindo o slide 6
                                                            // do link https://moodle.inf.ufrgs.br/pluginfile.php/199727/mod_resource/content/2/Aula_22_Laboratorio_5.pdf
        V = (position_model.y - miny) / (maxy - miny);
    }

        else {                  // Projeção Esférica

                                     
        vec4 p_vector;
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        p_vector = position_model - bbox_center;
        float p_vector_length = length(p_vector);
        float symbol_phi = asin(p_vector.y / p_vector_length);


        V = (symbol_phi + M_PI_2) / M_PI;

    
        }

        return V;
 }




