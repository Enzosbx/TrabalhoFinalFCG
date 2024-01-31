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

// Coordenadas de textura obtidas do arquivo OBJ
in vec2 textcoords;

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

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923





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

    // Coordenadas de textura U e V
    
    float U = 0.0;
    float V = 0.0;



    // Vetor que define o sentido da reflexão especular ideal.   // ch
    vec4 r = vec4(0.0,0.0,0.0,0.0); 
    r = 2 * n * dot(n,l) - l;

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

 
    if ( object_id == STONE_EYES )          // Olhos serão especulares   // ch
    {
        
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

    else if ( object_id == REAPER )   // ch
    {
        float minx = bbox_min.x;    // Projeção planar
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

                                                            // seguindo o slide 6
                                                            // do link https://moodle.inf.ufrgs.br/pluginfile.php/199727/mod_resource/content/2/Aula_22_Laboratorio_5.pdf
        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);
        
    }

    
    else if ( object_id == SCORPION )   // ch
    {
        float minx = bbox_min.x;    // Projeção planar
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

                                                            // seguindo o slide 6
                                                            // do link https://moodle.inf.ufrgs.br/pluginfile.php/199727/mod_resource/content/2/Aula_22_Laboratorio_5.pdf
        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);
       
    }


     else if (object_id == FENCEA) 
    {
                       // Projeção Esférica
        vec4 p_vector;      
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        p_vector = position_model - bbox_center;
        float p_vector_length = length(p_vector);


        float symbol_theta = atan(p_vector.x,p_vector.z);
        float symbol_phi = asin(p_vector.y / p_vector_length);


        U = (symbol_theta + M_PI) / (2 * M_PI);
        V = (symbol_phi + M_PI_2) / M_PI;
    }

     else if (object_id == FENCEB) 
    {
                       // Projeção Esférica
        vec4 p_vector;      
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        p_vector = position_model - bbox_center;
        float p_vector_length = length(p_vector);


        float symbol_theta = atan(p_vector.x,p_vector.z);
        float symbol_phi = asin(p_vector.y / p_vector_length);


        U = (symbol_theta + M_PI) / (2 * M_PI);
        V = (symbol_phi + M_PI_2) / M_PI;
    }


    else if (object_id == WALL_CUBE) 
    {
                                    // Projeção Esférica
        vec4 p_vector;      
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        p_vector = position_model - bbox_center;
        float p_vector_length = length(p_vector);


        float symbol_theta = atan(p_vector.x,p_vector.z);
        float symbol_phi = asin(p_vector.y / p_vector_length);


        U = (symbol_theta + M_PI) / (2 * M_PI);
        V = (symbol_phi + M_PI_2) / M_PI;

    }

     else if (object_id == FLOOR_CUBE) 
    {
                                   // Projeção Esférica
        vec4 p_vector;
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;

        p_vector = position_model - bbox_center;
        float p_vector_length = length(p_vector);


        float symbol_theta = atan(p_vector.x,p_vector.z);
        float symbol_phi = asin(p_vector.y / p_vector_length);


        U = (symbol_theta + M_PI) / (2 * M_PI);
        V = (symbol_phi + M_PI_2) / M_PI;

    }


    else // Objeto desconhecido = verde  // ch 
    {
        Kd = vec3(0.0,0.6,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }


    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);            // ch 


                  // tarefa 2.2 lab 4   --> falta atualizar para a lógica do jogo

    /*


    vec4 pos_spotlight = vec4(0.0f,2.0f,1.0f,1.0f);   // Posição da fonte spotlight
    vec4 dir_vect_spotlight = vec4(0.0f,-1.0f,0.0f,0.0f);                          // Vetor de direção da fonte spotlifgt
    float opening_angle = radians(30.0f);

    vec4 normalized_pvector = normalize(p - pos_spotlight);
    vec4 normalized_v = normalize(dir_vect_spotlight);


    float beta_cossine = dot(normalized_pvector, normalized_v);
    float alfa_cossine = cos(opening_angle);  // devemos dividir por 2, conforme a imagem do slide 211?? Se dividirmos fica diferente do resultado esperado pelo prof

    if (alfa_cossine > beta_cossine) {
        I = vec3(0.0,0.0,0.0);
    }

    */

    

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);        // ch

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = vec3(0.0,0.0,0.0); 
    lambert_diffuse_term = Kd * I * max (0, dot(n,l));


    // Termo ambiente
    vec3 ambient_term = vec3(0.0,0.0,0.0); 
    ambient_term = Ka * Ia;


    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = vec3(0.0,0.0,0.0); 
    phong_specular_term = Ks * I * pow (max (0, dot(r,v)) , q);


    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente

  
    if (object_id == REAPER) {

           // Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
       vec3 Kd2 = texture(TextureImage2, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd2 * (lambert + 0.01);
    }

    else if (object_id == WALL_CUBE) {

       vec3 Kd0 = texture(TextureImage0, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd0 * (lambert + 0.01);

    }

    else if (object_id == FENCEA) {

       vec3 Kd4 = texture(TextureImage4, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd4 * (lambert + 0.01);
    }

    else if (object_id == FENCEB) {

       vec3 Kd4 = texture(TextureImage4, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd4 * (lambert + 0.01);

    }

    else if (object_id == SCORPION) {
  
       vec3 Kd3 = texture(TextureImage3, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd3 * (lambert + 0.01);
    }

    else if (object_id == FLOOR_CUBE) {

       vec3 Kd1 = texture(TextureImage1, vec2(U,V)).rgb;

       // Equação de Iluminação
       float lambert = max(0,dot(n,l));

       color.rgb = Kd1 * (lambert + 0.01);

    }

    else {
    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;
    }


    color.a = 1;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);


} 

