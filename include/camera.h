#ifndef _CAMERA_H
#define _CAMERA_H


#pragma once

#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "matrices.h"


// Informações iniciais da camera

float g_ScreenRatio = 1.0f; 
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 10.5f; // Distância da câmera para a origem


bool g_UseLookAtCamera = true;


float camera_speed = 0;
float prev_time = 0;
glm::vec4 w_vector = {0.0f,0.0f,0.0f,0.0f};
glm::vec4 up_w_cross  = {0.0f,0.0f,0.0f,0.0f};
glm::vec4 u_vector  = {0.0f,0.0f,0.0f,0.0f};
glm::vec4 camera_movement = {0.0f,0.0f,0.0f,0.0f};
glm::vec4 camera_position_c = {0.0f,0.0f,0.0f,0.0f};
glm::vec4 camera_view_vector = {0.0f,0.0f,0.0f,0.0f};



glm::mat4 defineViewLACam(glm::mat4 view) {

    float r = g_CameraDistance;
    float y = r*sin(g_CameraPhi);
    float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
    float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

    glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)
    // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    camera_position_c  = glm::vec4(x,y,z,1.0f); // Ponto "c", centro da câmera
    glm::vec4 camera_lookat_l    = glm::vec4(0.0f,0.0f,0.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada
    view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

    return view;


}

glm::mat4 defineViewFCam(glm::mat4 view) {

    float r = g_CameraDistance;
    float y = r*sin(g_CameraPhi);
    float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
    float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

    glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    
    camera_speed = 0.2f/60; // Velocidade da câmera
    prev_time = (float)glfwGetTime();
    camera_position_c  = glm::vec4{x,y,z,1.0f} + camera_movement;
    camera_view_vector = {-x,-y,-z,0.0f};
    view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

          // Vetores utilizados para a movimentação da câmera!

    w_vector = -camera_view_vector / norm(camera_view_vector);
    up_w_cross = crossproduct(camera_up_vector, w_vector);
    u_vector = up_w_cross / norm(up_w_cross);

    return view;

}

#endif




