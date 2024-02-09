
#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <thread>

#include <Windows.h>
/*#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>*/

// Headers das bibliotecas OpenGL
#include <glad/glad.h>  // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h> // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <stb_image.h>
#include <tiny_obj_loader.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "callbacks.h"
#include "shaders.h"
#include "player.h"


// Definição de todos as figuras existentes nos objs.

#define GOLEM_EYES 1
#define GOLEM_HANDS_LEGS 2
#define GOLEM_HEAD 3
#define GOLEM_TORSO 4
#define REAPER 5
#define SCORPION 6
#define WALL_CUBE 7
#define FLOOR_CUBE 8
#define BULLETA 9
#define BULLETB 10
#define BULLETC 11
#define FAKE_CUBE 12
#define GUN 13
#define DIAMOND 14

GLFWwindow *window;

// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .
struct ObjModel
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char *filename, const char *basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, então setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretório dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i + 1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                        filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};

// Outras funções, que estão implementadas após a main.

void BuildTrianglesAndAddToVirtualScene(ObjModel *); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel *model);                // Computa normais de um ObjModel, caso não existam.
void LoadTextureImage(const char *filename);         // Função que carrega imagens de textura
void DrawVirtualObject(const char *object_name);     // Desenha um objeto armazenado em g_VirtualScene

// Funções e definições relativas ao mapa

void drawMap(glm::mat4 model); // Desenha o mapa do jogo
void readMap(FILE *arquivo);   // Lê o mapaa

// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string name;              // Nome do objeto
    size_t first_index;            // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t num_indices;            // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum rendering_mode;         // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3 bbox_min;            // Axis-Aligned Bounding Box do objeto
    glm::vec3 bbox_max;
};


// Abaixo, funções definidas antes da main

   // Funções relativas ao mecanismo de tiro e animação

double deltaTime()
{
    static double lastFrameTime = 0.0;
    double currentFrameTime = glfwGetTime();
    double deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
    return deltaTime;
}

    // Verificação de colisão da bala com os inimigos   
    // e dos inimigos e com o coletável

void HitBulletEnemy(Bullet *bullet, Enemy *enemy)
{
    glm::vec4 Inim = {enemy->pos.x, enemy->height_center, enemy->pos.y, 1.0f};
    if (DistanceDots(bullet->pos, Inim) <= enemy->raio && enemy->vivo == 1)
    {
        bullet->vivo = 0;

        enemy->vida--;
        enemy->veloc *= 1.05;
        enemy->scale *= 1.018;
        enemy->raio *= 1.018;
        if (enemy->vida <= 0)
        {
            enemy->vivo = 0;
            if (enemy->holdin != NumDiam)
            {
                diamonds[enemy->holdin].state = Floor;
                enemy->holdin = NumDiam;
            }
        }
    }
}
                                // Definição de colisão da bala com os blocos do mapa (cenário)
int HitBullet(Bullet *bullet)
{
    if (Labirinto[(int)(bullet->pos.x + 1.5 * lado_bloco) / lado_bloco][(int)(bullet->pos.z + 1.5 * lado_bloco) / lado_bloco].type == parede)
    {
        if (Labirinto[(int)(bullet->pos.x + 1.5 * lado_bloco) / lado_bloco][(int)(bullet->pos.z + 1.5 * lado_bloco) / lado_bloco].breakable == 1)
        {
            Labirinto[(int)(bullet->pos.x + 1.5 * lado_bloco) / lado_bloco][(int)(bullet->pos.z + 1.5 * lado_bloco) / lado_bloco].type = chao;
            Labirinto[(int)(bullet->pos.x + 1.5 * lado_bloco) / lado_bloco][(int)(bullet->pos.z + 1.5 * lado_bloco) / lado_bloco].breakable = 0;
        }
        return 1;
    }
    return 0;
}

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Número de texturas carregadas pela função LoadTextureImage()

GLuint g_NumLoadedTextures = 0;

// Matriz model e estrutura da bala
// tiveram que ser declaradas como variavel global

glm::mat4 model = Matrix_Identity();
Bullet bullet;

int main(int argc, char *argv[])
 {
    

    // Definição das dimensões da janela


    HWND hDesktop = GetDesktopWindow();
    RECT desktopRect;
    GetWindowRect(hDesktop, &desktopRect);

    int screenWidth = 1980;  // desktopRect.right;   
    int screenHeight = 1080; // desktopRect.bottom;

    // Leitura do arquivo contendo o mapa

    FILE *arquivo;
    readMap(arquivo); 

    int success = glfwInit();       // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
                                    // sistema operacional, onde poderemos renderizar com OpenGL.
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional

    window = glfwCreateWindow(screenWidth, screenHeight, "TrabFinal INF01047 - Enzo SBX e Geancarlo K", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenWidth, screenHeight, GLFW_DONT_CARE);

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, screenWidth, screenHeight); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glversion = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    LoadShadersFromFiles();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    // Carregamento das imagens de textura que serão utilizadas.

    LoadTextureImage("../../data/wood-texture3.jpg");        // TextureImage0
    LoadTextureImage("../../data/textura-pedra3.jpg");       // TextureImage1
    LoadTextureImage("../../data/textura-caveira.jpg");      // TextureImage2
    LoadTextureImage("../../data/textura-carne2.jpg");       // TextureImage3
    LoadTextureImage("../../data/textura-dourada3.jpg");     // TextureImage4
    LoadTextureImage("../../data/textura-bala2.jpg");        // TextureImage5
    LoadTextureImage("../../data/fake-wood-texture3.jpg");   // TextureImage6
    LoadTextureImage("../../data/textura-golem3.jpg");       // TextureImage7
    LoadTextureImage("../../data/outra-textura-golem2.jpg"); // TextureImage8
    LoadTextureImage("../../data/textura-diamante3.jpg");    // TextureImage9

    // Construímos a representação de objetos geométricos através de malhas de triângulos

    ObjModel stonemodel("../../data/Stone.obj");
    ComputeNormals(&stonemodel);
    BuildTrianglesAndAddToVirtualScene(&stonemodel);

    // NÃO UTILIZADO

    ObjModel planemodel("../../data/plane.obj");
    ComputeNormals(&planemodel);
    BuildTrianglesAndAddToVirtualScene(&planemodel);

    ObjModel scorpionmodel("../../data/Scorpion.OBJ");
    ComputeNormals(&scorpionmodel);
    BuildTrianglesAndAddToVirtualScene(&scorpionmodel);

    ObjModel reapermodel("../../data/Reaper.obj");
    ComputeNormals(&reapermodel);
    BuildTrianglesAndAddToVirtualScene(&reapermodel);

    // NÃO UTILIZADO

    ObjModel doormodel("../../data/door.obj");
    ComputeNormals(&doormodel);
    BuildTrianglesAndAddToVirtualScene(&doormodel);

    ObjModel wall_cubemodel("../../data/cube.obj");
    ComputeNormals(&wall_cubemodel);
    BuildTrianglesAndAddToVirtualScene(&wall_cubemodel);

    ObjModel floor_cubemodel = wall_cubemodel;
    ComputeNormals(&floor_cubemodel);
    BuildTrianglesAndAddToVirtualScene(&floor_cubemodel);

    ObjModel fake_cubemodel = wall_cubemodel;
    ComputeNormals(&fake_cubemodel);
    BuildTrianglesAndAddToVirtualScene(&fake_cubemodel);

    ObjModel bullet_model("../../data/Bullet.obj");
    ComputeNormals(&bullet_model);
    BuildTrianglesAndAddToVirtualScene(&bullet_model);

    ObjModel gun_model("../../data/Gun.obj");
    ComputeNormals(&gun_model);
    BuildTrianglesAndAddToVirtualScene(&gun_model);

    ObjModel diamond_model("../../data/diamond.obj");
    ComputeNormals(&diamond_model);
    BuildTrianglesAndAddToVirtualScene(&diamond_model);

    if (argc > 1)
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glLoadIdentity();


    float prev_time_d = glfwGetTime();  // variavel utilizada na animação da arma.


    bullet.vivo = 0;

    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
   
    while (!glfwWindowShouldClose(window))
    {
        double dt = deltaTime();

        // Lógica de atualização e renderização do jogo aqui

        // Limita o FPS
        double targetFrameTime = 1.0 / 120.0; // 60 FPS
        if (dt < targetFrameTime)
        {
            double sleepTime = targetFrameTime - dt;
            glfwWaitEventsTimeout(sleepTime);
        }

        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


        for (int i = 0; i < NumEnemies; i++)  // Iniciamos o movimento dos inimigos
        {
            WalkEnemy(&enemy[i]);
        }

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);


        walk(&distance);
        if (hurt)
            recover_time--;
        for (int i = 0; i < NumEnemies; i++)   // verifica se houve colisão entre o jogador (câmera)
        {                                       // e inimigo
            player_enemy_collision(&enemy[i]);
        }
        


        //  Computamos a matriz "View" utilizando os parâmetros da câmera para
        //  definir o sistema de coordenadas da câmera.

        glm::mat4 view;

        if (g_UseLookAtCamera)
        { // Look At Camera
            view = defineViewLACam(view);
        }
        else
        { // Free Camera
            view = defineViewFCam(view);
        }

        // Se o usuário tiver capturado a arma, ele pode atirar, com o botão esquerdo do mouse

        if (g_LeftMouseButtonPressed && canShoot && picked_gun) 
        { // tiro
            bullet = newBullet();
            canShoot = 0;
        }
        else if (g_LeftMouseButtonPressed == false)
            canShoot = 1;


        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo!
        float nearplane = -1.0f;  // Posição do "near plane"
        float farplane = -400.0f; // Posição do "far plane"

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;
        projection = defineProjection(projection, nearplane, farplane);

        model = Matrix_Identity(); // Transformação identidade de modelagem
        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

        
        // Desenhamos o mapa, com seus cubos
        drawMap(model);

        // Renderização da bala. "bullet.vivo"
        // indica se ela ainda está em movimento, ou seja, se não colidiu com
        // nenhum outro objeto do mapa (bloco ou inimigo)

        if (bullet.vivo)
        {
            if (bullet.time >= 0)
            {
                bullet.time -= 1;
                bullet.pos += bullet.veloc * bullet.dir;

                //  MOSTRAR BALA

                model = Matrix_Translate(bullet.pos.x, bullet.pos.y, bullet.pos.z) * Matrix_Rotate_Y(g_CameraTheta + pi) * Matrix_Translate(-2, -1.7, 2.5) * Matrix_Rotate_X(pi / 2 + g_CameraPhi) * Matrix_Scale(1.5f, 1.5f, 1.5f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, BULLETA);
                DrawVirtualObject("the_bulleta");

                model = Matrix_Translate(bullet.pos.x, bullet.pos.y, bullet.pos.z) * Matrix_Rotate_Y(g_CameraTheta + pi) * Matrix_Translate(-2, -1.7, 2.5) * Matrix_Rotate_X(pi / 2 + g_CameraPhi) * Matrix_Scale(1.5f, 1.5f, 1.5f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, BULLETB);
                DrawVirtualObject("the_bulletb");

                model = Matrix_Translate(bullet.pos.x, bullet.pos.y, bullet.pos.z) * Matrix_Rotate_Y(g_CameraTheta + pi) * Matrix_Translate(-2, -1.7, 2.5) * Matrix_Rotate_X(pi / 2 + g_CameraPhi) * Matrix_Scale(1.5f, 1.5f, 1.5f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, BULLETC);
                DrawVirtualObject("the_bulletc");

                if (HitBullet(&bullet))     // Se houve a colisão com o cenário
                    bullet.vivo = 0;        // deixamos de renderizar

                for (int i = 0; i < NumEnemies; i++)
                {                                      // Verificamos se houve a colisão 
                    if (bullet.vivo == 1)                  // com o inimigo
                        HitBulletEnemy(&bullet, &enemy[i]);
                }
            }
            else
                bullet.vivo = 0;
        }

        // Renderização dos inimigos no mapa, se ainda estiverem vivos

        for (int i = 0; i < NumEnemies; i++)
        {
            if (enemy[i].vivo)
            {
                switch (enemy[i].type)
                {
                case Scorpion:

                    model = Matrix_Translate(enemy[i].pos.x, -12.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i])) * Matrix_Rotate_X(-1.5708f) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, SCORPION);
                    DrawVirtualObject("the_scorpion");
                    break;
                case Golem:
                    model = Matrix_Translate(enemy[i].pos.x, -17.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i]) + pi) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, GOLEM_EYES);
                    DrawVirtualObject("eyes_Esfera.007");
                    model = Matrix_Translate(enemy[i].pos.x, -17.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i]) + pi) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, GOLEM_HEAD);
                    DrawVirtualObject("head.001_ice.004");
                    model = Matrix_Translate(enemy[i].pos.x, -17.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i]) + pi) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, GOLEM_HANDS_LEGS);
                    DrawVirtualObject("hands&leg.001_ice.003");
                    model = Matrix_Translate(enemy[i].pos.x, -17.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i]) + pi) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, GOLEM_TORSO);
                    DrawVirtualObject("torso.001_ice.005");
                    break;
                case Reaper:
                    model = Matrix_Translate(enemy[i].pos.x, -19.5f + enemy[i].floating * (1 + sin(enemy[i].percent * pi / 50)), enemy[i].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[i]) + pi) * Matrix_Rotate_X(-1.5708f) * Matrix_Scale(enemy[i].scale, enemy[i].scale, enemy[i].scale);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, REAPER);
                    DrawVirtualObject("the_reaper");
                    break;
                }
            }
        }

        // Renderizamos o modelo da arma, com animação

        float time_now = glfwGetTime();
        float delta_t = time_now - prev_time_d;
        prev_time = time_now;

        glm::vec4 pos_gun = {0.0f, 0.0f, 100.0f, 1.0f};
        if (picked_gun == 0)
        {
            model = Matrix_Translate(pos_gun.x, pos_gun.y, pos_gun.z) * Matrix_Rotate_Y(g_AngleY + delta_t) * Matrix_Scale(8.0f, 8.0f, 8.0f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, GUN);
            DrawVirtualObject("the_gun");
            if (DistanceDots(pos_gun, camera_position_c) <= 20)
                picked_gun = 1;
        }
        else
        {
            model = Matrix_Translate(camera_position_c.x, camera_position_c.y - 2, camera_position_c.z) * Matrix_Rotate_Y(g_CameraTheta - pi / 2) * Matrix_Translate(-2, -0.5, -2) * Matrix_Rotate_Z(g_CameraPhi) * Matrix_Scale(8.0f, 8.0f, 8.0f);
            glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, GUN);
            DrawVirtualObject("the_gun");
        }

        // DESENHAMOS O MODELO DO DIAMANTE

        for (int i = 0; i < NumDiam; i++)
        {
            glm::vec4 diam_pos = {(diamonds[i].pos.x - 1) * lado_bloco, 0.0f, (diamonds[i].pos.y - 1) * lado_bloco, 1.0f};
            if (DistanceDots(camera_position_c, diam_pos) <= diamonds[i].raio && diamonds[i].state == Floor)
                diamonds[i].state = Holden;
            if (diamonds[i].state == Floor)
            {
                model = Matrix_Translate((diamonds[i].pos.x - 1) * lado_bloco, -13.0f, (diamonds[i].pos.y - 1) * lado_bloco) * Matrix_Rotate_Y(delta_t) * Matrix_Scale(8.0f, 8.0f, 8.0f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, DIAMOND);
                DrawVirtualObject("the_diamond");
            }
            else if (diamonds[i].state == Holden)
            {
                model = Matrix_Translate(camwalk.x, camwalk.y - 2, camwalk.z) * Matrix_Rotate_Y(g_CameraTheta) * Matrix_Translate(-1.75f + 0.5 * i, 3, -2) * Matrix_Rotate_Y(delta_t) * Matrix_Scale(0.2f, 0.2f, 0.2f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, DIAMOND);
                DrawVirtualObject("the_diamond");
            }
            else if (diamonds[i].state == Stolen)
            {
                int inim = NumDiam;
                int scorp = 0;
                int j;
                for (j = 0; j < NumEnemies; j++)
                {
                    if (enemy[j].holdin == i)
                    {
                        inim = j;
                    }
                }
                model = Matrix_Translate(enemy[inim].pos.x, -13.0f, enemy[inim].pos.y) * Matrix_Rotate_Y(rotacaoEnemy(enemy[inim]) + pi) * Matrix_Translate(0.0f, 5.0f, 10.0f) * Matrix_Scale(8.0f, 8.0f, 8.0f);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, DIAMOND);
                DrawVirtualObject("the_diamond");
            }
        }

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics

        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char *object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);

    // Setamos as variáveis "bbox_min" e "bbox_max" do fragment shader
    // com os parâmetros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de  ???   ---> isso é de algo anterior
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].rendering_mode,
        g_VirtualScene[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void *)(g_VirtualScene[object_name].first_index * sizeof(GLuint)));

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel *model) // fazmeos essa chamada para cada modelo/objeto do arquivo obj!!!!.
{
    GLuint vertex_array_object_id; // Criação e ligação do VAO
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float> model_coefficients;
    std::vector<float> normal_coefficients;
    std::vector<float> texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval, maxval, maxval);
        glm::vec3 bbox_max = glm::vec3(minval, minval, minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];

                indices.push_back(first_index + 3 * triangle + vertex);

                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                // printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back(vx);   // X
                model_coefficients.push_back(vy);   // Y
                model_coefficients.push_back(vz);   // Z
                model_coefficients.push_back(1.0f); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if (idx.normal_index != -1)
                {
                    const float nx = model->attrib.normals[3 * idx.normal_index + 0];
                    const float ny = model->attrib.normals[3 * idx.normal_index + 1];
                    const float nz = model->attrib.normals[3 * idx.normal_index + 2];
                    normal_coefficients.push_back(nx);   // X
                    normal_coefficients.push_back(ny);   // Y
                    normal_coefficients.push_back(nz);   // Z
                    normal_coefficients.push_back(0.0f); // W
                }

                if (idx.texcoord_index != -1)
                {
                    const float u = model->attrib.texcoords[2 * idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2 * idx.texcoord_index + 1];
                    texture_coefficients.push_back(u);
                    texture_coefficients.push_back(v);
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject theobject;
        theobject.name = model->shapes[shape].name;
        theobject.first_index = first_index;                  // Primeiro índice
        theobject.num_indices = last_index - first_index + 1; // Número de indices
        theobject.rendering_mode = GL_TRIANGLES;              // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
        theobject.vertex_array_object_id = vertex_array_object_id;

        theobject.bbox_min = bbox_min;
        theobject.bbox_max = bbox_max;

        g_VirtualScene[model->shapes[shape].name] = theobject;
    }

    // VBO GEOMETRIA  ---> as informacoes estao dentro do obj

    GLuint VBO_model_coefficients_id; // Criação de VBO para coordenadas ? (locais?)
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0;            // "(location = 0)" em "shader_vertex.glsl"
    GLint number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!normal_coefficients.empty()) // pode haver ou nao normais
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1;             // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!texture_coefficients.empty()) // pode haver ou não texturas
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2;             // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // VBO TOPOLOGIA        // ---> as informacoes estao dentro do obj!

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que carrega uma imagem para ser utilizada como textura
void LoadTextureImage(const char *filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if (data == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

void drawMap(glm::mat4 model)
{
    for (int i = 0; i < DimLab; i++)
    {
        for (int j = 0; j < DimLab; j++)
        {
            if (Labirinto[i][j].type == parede)
            {
                if (Labirinto[i][j].breakable)
                {
                    // MODELO DO CUBO FALSO

                    model = Matrix_Translate(lado_bloco * (i - 1), altura_bloco - 19.5, lado_bloco * (j - 1)) * Matrix_Scale(lado_bloco, altura_bloco, lado_bloco);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, FAKE_CUBE);
                    DrawVirtualObject("the_cube");
                }
                else
                {
                    model = Matrix_Translate(lado_bloco * (i - 1), altura_bloco - 19.5, lado_bloco * (j - 1)) * Matrix_Scale(lado_bloco, altura_bloco, lado_bloco);
                    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(g_object_id_uniform, WALL_CUBE);
                    DrawVirtualObject("the_cube");
                }
            }
            if (Labirinto[i][j].type == chao)
            {
                model = Matrix_Translate(lado_bloco * (i - 1), -17.5 - altura_bloco / 2, lado_bloco * (j - 1) + lado_bloco / 2) * Matrix_Scale(lado_bloco, altura_bloco, lado_bloco) * Matrix_Rotate_X(pi / 2);
                glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, FLOOR_CUBE);
                DrawVirtualObject("the_cube");
            }
        }
    }
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel *model)
{
    if (!model->attrib.normals.empty())
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4 vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx, vy, vz, 1.0);
            }

            const glm::vec4 a = vertices[0];
            const glm::vec4 b = vertices[1];
            const glm::vec4 c = vertices[2];

            // cálculo da normal de um triângulo cujos vértices
            // estão nos pontos "a", "b", e "c", definidos no sentido anti-horário.

            const glm::vec4 vab = b - a;
            const glm::vec4 vac = c - a;
            const glm::vec4 n = crossproduct(vab, vac);

            // Slide 163 das Aulas 17 e 18 - Modelos de Iluminação

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize(3 * num_vertices);

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3 * i + 0] = n.x;
        model->attrib.normals[3 * i + 1] = n.y;
        model->attrib.normals[3 * i + 2] = n.z;
    }
}

