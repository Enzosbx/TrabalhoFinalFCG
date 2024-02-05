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
#include <time.h>

// #include "enemy.h"
#define DimLab 19
#define lado_bloco 50
#define altura_bloco 35




float radius_player = 8;
float pi = 3.14159265;
int canShoot = 1;
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
    int time = 1000;
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
} Enemy;

#define NumEnemies 9
Enemy enemy[NumEnemies];
Enemy CreateEnemy(int x1, int y1, tipoEnemy tipo);

Bullet newBullet()
{
    Bullet bullet;
    bullet.vivo = 1;
    bullet.pos = camera_position_c;
    bullet.dir = camera_view_vector;
    bullet.pos.y -= 2;
    bullet.veloc = 0.4f;
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

void walk()
{

    if (w_key_pressed == true)
    {
        camera_movement += glm::vec4{-w_vector.x * norm2D(), 0.0f, -w_vector.z * norm2D(), w_vector.w} * camera_speed;
        // camera_movement += -w_vector * camera_speed;
    }
    if (a_key_pressed == true)
    {
        camera_movement += -u_vector * camera_speed;
        // camera_movement += -u_vector * camera_speed;
    }
    if (s_key_pressed == true)
    {
        camera_movement += glm::vec4{w_vector.x * norm2D(), 0.0f, w_vector.z * norm2D(), w_vector.w} * camera_speed;
        // camera_movement += w_vector * camera_speed;
    }
    if (d_key_pressed == true)
    {
        camera_movement += u_vector * camera_speed;
        // camera_movement += u_vector * camera_speed;
    }
    cam_colisoes();
}

void readMap(FILE *arquivo)
{
    // Abre o arquivo usando um caminho relativo.

    arquivo = fopen("../../labirinto.txt", "r+");
    int inimigo = 0;
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
    switch (tipo)
    {
    case Scorpion:
        enemy.vida = 4;
        enemy.raio = 10;
        enemy.type = Scorpion;
        enemy.veloc = 0.4f;
        enemy.floating = 2;
        break;
    case Golem:
        enemy.vida = 8;
        enemy.raio = 25;
        enemy.type = Golem;
        enemy.veloc = 0.2f;
        enemy.floating = 1;
        break;
    case Reaper:
        enemy.vida = 2;
        enemy.raio = 20;
        enemy.type = Reaper;
        enemy.veloc = 0.8f;
        enemy.floating = 0;

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
    // printf("%f\n", r);
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
    return;
}

#endif