#pragma once

#ifndef _ENEMY_H
#define _ENEMY_H

#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stack>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include "player.h"

#define lado_bloco 50
#define altura_bloco 35


typedef struct
{
    float x;
    float y;
} Vetor2;

typedef enum
{
    Norte = 0,
    Leste,
    Sul,
    Oeste,
    Centro
} Lado;
typedef struct
{
    Lado dir_i;
    Lado dir_f;
    float percent;
    Vetor2 pos;
    Vetor2 tile;
} Enemy;

Enemy CreateEnemy(int x1, int y1)
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
    return enemy;
}

Vetor2 porcento(Vetor2 inicio, Vetor2 fim, int porcento)
{
    Vetor2 pos;
    pos.x = (inicio.x * (100 - porcento) + fim.x * (porcento)) / 100;
    pos.y = (inicio.y * (100 - porcento) + fim.y * (porcento)) / 100;
    return pos;
}

Vetor2 PosBloco(float x1, float y1, Lado lado)
{
    Vetor2 pos;
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

Lado NovaDirecao(Enemy enemy)
{

    for (int tent = 0; tent < 50; tent++)
    {
        switch (rand() % 4)
        {
        case Norte:
            if (Norte != enemy.dir_i && Labirinto[(int)enemy.tile.x][(int)enemy.tile.y - 1] == chao)
            {
                return Norte;
            }
            break;
        case Sul:
            if (Norte != enemy.dir_i && Labirinto[(int)enemy.tile.x][(int)enemy.tile.y + 1] == chao)
            {
                return Sul;
            }
            break;
        case Leste:
            if (Norte != enemy.dir_i && Labirinto[(int)enemy.tile.x + 1][(int)enemy.tile.y] == chao)
            {
                return Leste;
            }
            break;
        case Oeste:
            if (Norte != enemy.dir_i && Labirinto[(int)enemy.tile.x - 1][(int)enemy.tile.y] == chao)
            {
                return Oeste;
            }
            break;
        }
    }

    return enemy.dir_i;
}

void WalkEnemy(Enemy *enemy, float veloc)
{
    srand(time(NULL));
    enemy->percent += veloc;
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
    printf("%f\n", enemy->tile.y);
    Vetor2 porcentoI, porcentoF;
    porcentoI = porcento(PosBloco(enemy->tile.x, enemy->tile.y, enemy->dir_i), PosBloco(enemy->tile.x, enemy->tile.y, Centro), enemy->percent);
    porcentoF = porcento(PosBloco(enemy->tile.x, enemy->tile.y, Centro), PosBloco(enemy->tile.x, enemy->tile.y, enemy->dir_f), enemy->percent);
    enemy->pos = porcento(porcentoI, porcentoF, enemy->percent);
    return;
}

#endif
