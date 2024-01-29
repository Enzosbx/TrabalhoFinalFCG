

#ifndef _MAP_H

#define _MAP_H


#pragma once


#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>




#define DimLab 19
int Labirinto[DimLab][DimLab];

void readMap(FILE *arquivo);  // Lê o mapa



void readMap(FILE *arquivo) {
    // Abre o arquivo usando um caminho relativo.

    arquivo = fopen("../../labirinto.txt", "r+");

    // Verifica se a abertura do arquivo foi bem-sucedida.
    if (arquivo == NULL)
    {
        printf("Nao foi possivel abrir o arquivo.\n");
        return; // Retorna um código de erro (er - mudar para 1);
    }
    else {
    printf("Blablabla.\n");
    }

    for (int i = 0; i < DimLab; i++)
    {
        for (int j = 0; j < DimLab; j++)
        {
            char c = fgetc(arquivo);
            switch (c)
            {
            case 'X':
                Labirinto[i][j] = 1;
                break;
            case ' ':
                Labirinto[i][j] = 0;
                break;
            }
        }
        fgetc(arquivo); // quebra de linha
    }
    fclose(arquivo);
}


#endif


