#pragma once

#ifndef _PLAYER_H
#define _PLAYER_H

#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stack>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include "callbacks.h"

#define DimLab 19
#define lado_bloco 45
#define altura_bloco 100
#define RECOVERTIME 300

int life = 10;
int recover_time = RECOVERTIME;
int hurt = 0;
int Running = 500;
float radius_player = 15;
float pi = 3.14159265;
int canShoot = 0;
int picked_gun = 0;
typedef enum
{
    chao,
    parede
} bloco;

typedef struct
{
    int x;
    int y;
} Vetor2i;
typedef struct
{
    bloco type;
    Vetor2i pos;
    int breakable;
} map;
typedef struct
{
    float x;
    float y;
} Vetor2f;

typedef struct
{
    glm::vec4 pos;
    glm::vec4 dir;
    int vivo;
    float veloc;
    int time = 100;
} Bullet;

typedef enum
{
    Norte = 0,
    Leste,
    Sul,
    Oeste,
    Centro
} Lado;
typedef enum
{
    Scorpion = 0,
    Golem,
    Reaper,
    tam_tipoEnemy
} tipoEnemy;
typedef struct
{
    Lado dir_i;
    Lado dir_f;
    float percent;
    Vetor2f pos;
    Vetor2f tile;
    int vivo;
    tipoEnemy type;
    int vida;
    int raio;
    float veloc;
    int floating;
    float height_center;
    float scale;
    int holdin;
} Enemy;

typedef enum {Floor, Holden, Stolen}Diam_state;

typedef struct
{
    Vetor2i pos;
    Diam_state state;
    float raio;
} Diamonds;

#define NumDiam 8
Diamonds diamonds[NumDiam];

#define NumEnemies 11
Enemy enemy[NumEnemies];
Enemy CreateEnemy(int x1, int y1, tipoEnemy tipo);

float DistanceDots(glm::vec4 P1, glm::vec4 P2)
{
    return sqrt(pow(P1.x - P2.x, 2) + pow(P1.y - P2.y, 2) + pow(P1.z - P2.z, 2));
}

Diamonds CreateDiamond(int i, int j)
{
    Diamonds d;
    d.pos.x = i;
    d.pos.y = j;
    d.state = Floor;
    d.raio = 18;
    return d;
}

Bullet newBullet()
{
    Bullet bullet;
    bullet.vivo = 1;
    bullet.pos = camera_position_c;
    bullet.dir = camera_view_vector;
    bullet.veloc = 0.8f;
    bullet.time = 200;
    return bullet;
}

map Labirinto[DimLab][DimLab];

void cam_colisoes()
{
    Vetor2i tile;
    tile.x = (int)(camera_movement.x + lado_bloco / 2) / lado_bloco + 1;
    tile.y = (int)(camera_movement.z + lado_bloco / 2) / lado_bloco + 1;
    if ((int)(camera_movement.x + lado_bloco / 2) % (int)lado_bloco >= lado_bloco - radius_player && Labirinto[tile.x + 1][tile.y].type == parede)
    {
        // Colisao leste
        camera_movement.x -= (int)(camera_movement.x + lado_bloco / 2) % (int)lado_bloco + radius_player - lado_bloco;
        camera_movement.x = floor(camera_movement.x);
    }
    if ((int)(camera_movement.x + lado_bloco / 2) % (int)lado_bloco <= radius_player && Labirinto[tile.x - 1][tile.y].type == parede)
    {
        // Colisao oeste
        camera_movement.x -= (int)(camera_movement.x + lado_bloco / 2) % (int)lado_bloco - radius_player;
        camera_movement.x = ceil(camera_movement.x);
    }
    if ((int)(camera_movement.z + lado_bloco / 2) % (int)lado_bloco >= lado_bloco - radius_player && Labirinto[tile.x][tile.y + 1].type == parede)
    {
        // Colisao sul
        camera_movement.z -= (int)(camera_movement.z + lado_bloco / 2) % (int)lado_bloco + radius_player - lado_bloco;
        camera_movement.z = floor(camera_movement.z);
    }
    if ((int)(camera_movement.z + lado_bloco / 2) % (int)lado_bloco <= radius_player && Labirinto[tile.x][tile.y - 1].type == parede)
    {
        // Colisao norte
        camera_movement.z -= (int)(camera_movement.z + lado_bloco / 2) % (int)lado_bloco - radius_player;
        camera_movement.z = ceil(camera_movement.z);
    }
}

void player_enemy_collision(Enemy *enemy)
{

    glm::vec4 inimigo = {enemy->pos.x, enemy->height_center, enemy->pos.y, 1.0f};
    if (hurt)
    {
        if (recover_time <= 0)
        {
            recover_time == RECOVERTIME;
            hurt = 0;
        }
    }
    else if (DistanceDots(inimigo, camera_position_c) <= radius_player + enemy->raio)
    {
        hurt = 1;
        for(int i = 0; i < NumDiam; i++)
        {
            if(diamonds[i].state == Holden && enemy->holdin == NumDiam && enemy->vivo == true)
            {
                
                enemy->holdin = i;
                diamonds[i].state = Stolen;
                i = NumDiam;
            }
        }
    }
}

void walk(float *distance)
{
    int walking = 0, run = 0;
    if (left_shift_key_pressed == true && Running > 0)
    {
        camera_speed = 0.7f;
        Running--;
        if (Running < 0)
            Running = 0;
        run = 1;
    }
    else if (left_shift_key_pressed == false)
    {
        camera_speed = 0.5f;
        Running++;
        if (Running > 500)
            Running = 500;
    }
    float PressedButtons = (w_key_pressed || s_key_pressed) + (a_key_pressed || d_key_pressed);
    //printf("%d", PressedButtons);
    if (w_key_pressed == true)
    {
        camera_movement += glm::vec4{-w_vector.x * norm2D(), 0.0f, -w_vector.z * norm2D(), w_vector.w} * camera_speed/sqrt(PressedButtons);
        walking = 1;
    }
    if (a_key_pressed == true)
    {
        camera_movement += -u_vector * camera_speed/sqrt(PressedButtons);
        walking = 1;
    }
    if (s_key_pressed == true)
    {
        camera_movement += glm::vec4{w_vector.x * norm2D(), 0.0f, w_vector.z * norm2D(), w_vector.w} * camera_speed/sqrt(PressedButtons);
        walking = 1;
    }
    if (d_key_pressed == true)
    {
        camera_movement += u_vector * camera_speed/sqrt(PressedButtons);
        walking = 1;
    }
    if (walking == 1)
    {
        *distance += 0.1f;
        if (run == 1)
            *distance += 0.05f;
    }

    cam_colisoes();
}

void readMap(FILE *arquivo)
{
    // Abre o arquivo usando um caminho relativo.

    arquivo = fopen("../../labirinto.txt", "r+");
    int inimigo = 0;
    int rock_number = 0;
    // Verifica se a abertura do arquivo foi bem-sucedida.
    if (arquivo == NULL)
    {
        printf("Nao foi possivel abrir o arquivo.\n");
        return; // Retorna um código de erro (er - mudar para 1);
    }

    for (int i = 0; i < DimLab; i++)
    {
        for (int j = 0; j < DimLab; j++)
        {
            Labirinto[i][j].pos.x = lado_bloco * i - lado_bloco / 2;
            Labirinto[i][j].pos.y = lado_bloco * j - lado_bloco / 2;
            char c = fgetc(arquivo);
            Labirinto[i][j].breakable = 0;
            switch (c)
            {
            case 'X':
                Labirinto[i][j].type = parede;
                break;
            case ' ':
                Labirinto[i][j].type = chao;
                break;
            case 'B':
                Labirinto[i][j].type = parede;
                Labirinto[i][j].breakable = 1;
                break;
            case '0':
                Labirinto[i][j].type = chao;
                enemy[inimigo] = CreateEnemy(i, j, Scorpion);
                inimigo++;
                break;
            case '1':
                Labirinto[i][j].type = chao;
                enemy[inimigo] = CreateEnemy(i, j, Golem);
                inimigo++;
                break;
            case '2':
                Labirinto[i][j].type = chao;
                enemy[inimigo] = CreateEnemy(i, j, Reaper);
                inimigo++;
                break;
            case 'D':
                Labirinto[i][j].type = chao;
                diamonds[rock_number] = CreateDiamond(i, j);
                rock_number++;
                break;
            }
        }
        fgetc(arquivo); // quebra de linha
    }
    fclose(arquivo);
}

/////////////////////////////////////////ENEMY///////////////////////////////////

Enemy CreateEnemy(int x1, int y1, tipoEnemy tipo)
{
    x1--;
    y1--;
    x1 *= lado_bloco;
    y1 *= lado_bloco;
    Enemy enemy;
    enemy.dir_i = Norte;
    enemy.dir_f = Norte;
    enemy.percent = 0;
    enemy.pos.x = x1;
    enemy.pos.y = y1;
    enemy.tile.x = (int)(x1 + lado_bloco / 2) / lado_bloco + 1;
    enemy.tile.y = (int)(y1 + lado_bloco / 2) / lado_bloco + 1;
    enemy.vivo = 1;
    enemy.holdin = NumDiam;
    switch (tipo)
    {
    case Scorpion:
        enemy.vida = 8;
        enemy.raio = 12;
        enemy.type = Scorpion;
        enemy.veloc = 1.8f;
        enemy.floating = 2;
        enemy.height_center = -15;
        enemy.scale = 5;
        break;
    case Golem:
        enemy.vida = 20;
        enemy.raio = 25;
        enemy.type = Golem;
        enemy.veloc = 0.6f;
        enemy.floating = 1;
        enemy.height_center = 0;
        enemy.scale = 3;
        break;
    case Reaper:
        enemy.vida = 15;
        enemy.raio = 18;
        enemy.type = Reaper;
        enemy.veloc = 1.2f;
        enemy.floating = 0;
        enemy.height_center = 0;
        enemy.scale = 3;
        break;
    };
    return enemy;
}

void rotdir(float *angulo, Lado dir)
{
    switch (dir)
    {
    case Norte:
        *angulo = 0;
        break;
    case Leste:
        *angulo = -pi / 2;
        break;
    case Sul:
        *angulo = pi;
        break;
    case Oeste:
        *angulo = pi / 2;
        break;
    }
}

Vetor2f porcento(Vetor2f inicio, Vetor2f fim, int porcento)
{
    Vetor2f pos;
    pos.x = (inicio.x * (100 - porcento) + fim.x * (porcento)) / 100;
    pos.y = (inicio.y * (100 - porcento) + fim.y * (porcento)) / 100;
    return pos;
}

Vetor2f PosBloco(float x1, float y1, Lado lado)
{
    Vetor2f pos;
    pos.x = (x1 - 1) * lado_bloco;
    pos.y = (y1 - 1) * lado_bloco;
    switch (lado)
    {
    case Norte:
        pos.y -= lado_bloco / 2 + 1;
        break;
    case Sul:
        pos.y += lado_bloco / 2 - 1;
        break;
    case Leste:
        pos.x += lado_bloco / 2 - 1;
        break;
    case Oeste:
        pos.x -= lado_bloco / 2 + 1;
        break;
    default:
        break;
    }
    return pos;
}

Lado Lado_oposto(Lado lado)
{
    if (lado == Norte)
        return Sul;
    if (lado == Sul)
        return Norte;
    if (lado == Oeste)
        return Leste;
    if (lado == Leste)
        return Oeste;
    return Centro;
}
float rotacaoEnemy(Enemy enemy)
{
    float inicio, final;
    rotdir(&inicio, Lado_oposto(enemy.dir_i));
    rotdir(&final, enemy.dir_f);
    if (inicio - final > pi)
    {
        inicio -= 2 * pi;
    }
    else if (final - inicio > pi)
    {
        final -= 2 * pi;
    }
    float r = enemy.percent * final + (100 - enemy.percent) * inicio;

    return r / 100;
}
Lado NovaDirecao(Enemy enemy)
{

    for (int tent = 0; tent < 50; tent++)
    {
        switch (rand() % 4)
        {
        case Norte:
            if (Norte != enemy.dir_i && Labirinto[(int)enemy.tile.x][(int)enemy.tile.y - 1].type == chao)
            {
                return Norte;
            }
            break;
        case Sul:
            if (Sul != enemy.dir_i && Labirinto[(int)enemy.tile.x][(int)enemy.tile.y + 1].type == chao)
            {
                return Sul;
            }
            break;
        case Leste:
            if (Leste != enemy.dir_i && Labirinto[(int)enemy.tile.x + 1][(int)enemy.tile.y].type == chao)
            {
                return Leste;
            }
            break;
        case Oeste:
            if (Oeste != enemy.dir_i && Labirinto[(int)enemy.tile.x - 1][(int)enemy.tile.y].type == chao)
            {
                return Oeste;
            }
            break;
        }
    }
    return enemy.dir_i;
}

void WalkEnemy(Enemy *enemy)
{
    srand(time(NULL));
    enemy->percent += enemy->veloc;
    if (enemy->percent >= 100)
    {
        // Caso ele termine de andar naquele bloco
        enemy->percent = 0;
        // enemy->pos.x+=lado_bloco;
        enemy->pos.y += lado_bloco;
        /*enemy->tile.x = (int)(enemy->pos.x + lado_bloco / 2) / lado_bloco + 1;
        enemy->tile.y++;*/
        switch (enemy->dir_f)
        {
        case Norte:
            enemy->tile.y--;
            break;
        case Oeste:
            enemy->tile.x--;
            break;
        case Leste:
            enemy->tile.x++;
            break;
        case Sul:
            enemy->tile.y++;
            break;
        }
        enemy->dir_i = Lado_oposto(enemy->dir_f);
        enemy->dir_f = NovaDirecao(*enemy);
    }
    Vetor2f porcentoI, porcentoF;
    porcentoI = porcento(PosBloco(enemy->tile.x, enemy->tile.y, enemy->dir_i), PosBloco(enemy->tile.x, enemy->tile.y, Centro), enemy->percent);
    porcentoF = porcento(PosBloco(enemy->tile.x, enemy->tile.y, Centro), PosBloco(enemy->tile.x, enemy->tile.y, enemy->dir_f), enemy->percent);
    enemy->pos = porcento(porcentoI, porcentoF, enemy->percent);
    if(enemy->holdin != NumDiam && diamonds[enemy->holdin].state == Stolen)
    {
        diamonds[enemy->holdin].pos.x = enemy->tile.x;
        diamonds[enemy->holdin].pos.y = enemy->tile.y;
    }
    return;
}

#endif
