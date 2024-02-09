
# Alunos
**ENZO SERGI BERQUO XAVIER** e 
**GEANCARLO KOZENIESKI**


# Visão Geral 

Jogo estilo "FPS" desenvolvido como trabalho final da disciplina de Fundamentos de Computação Gráfica (INF01047) do curso de Ciência da Computação UFRGS, ministrada pelo 
professor Eduardo S.L Gastal. 

O objetivo central do jogo é coletar todos os diamantes espalhadas por um labirinto. Porém, nele existem diferentes inimigos (Golens, Escorpiões, Ceifadores) , que são capazes de roubar as joias da posse do jogador e fugir com elas.
O jogador é equipado com uma arma, e deve atirar e matar os vilões para conseguir recuperar as joias.

# Desenvolvimento e aplicação de conceitos de Computação Gráfica

No desenvolvimento do jogo, foram implementados os seguintes conceitos de Computação Gráfica

- **Objetos virtuais representados através de malhas poligonais complexas:**
   Há o carregamento de 6 objetos virtuais (Cubo, Ceifador, Arma,  Diamante, Escorpião, Golem) definidos por malhas complexas, a partir de arquivos .obj. 


- **Equações de Modelos de Iluminação de Objetos Virtuais:**
Para a iluminação da maioria dos objetos, foi utilizado o modelo de iluminação de Lambert visto em aula; as únicas exceções foram o Cubo Falso (FAKE_CUBE) e o inimigo Ceifador (REAPER, que utilizaram modelo de iluminação de Blinn Phong)


- **Modelos de Interpolação Para Iluminação:**
Para todos os objetos foi utilizado o modelo de interpolação de Phong no fragment_shader, com exceção do Ceifador, em que foi usado o modelo de Gouraud no vertex_shader.


- **Carregamento e Mapeamento de Texturas:**
Foi feito para todos os objetos a partir de imagens jpg. , que foram utilizados para a definição do Kd (coef. refletância difusa) utilizado nos modelos de iluminação. As coordenadas de texturas foram definidas por projeção esférica, ou projeção planar, ou carregdas do .obj, a depender do objeto.


- **Transformações geométricas de objetos virtuais:**
Depois que o jogador capturar a arma, são feitas  transformações de translação da arma, que acompanha a câmera enquanto ela se movimenta pelas teclas W/A/S/D


- **Controle de câmeras virtuais:**
O nosso jogo inicia com câmera livre, que é utilizada em todo o mecanismo do jogo. Há também câmera look-at, para a qual não encontramos um uso específico no jogo. Portanto, ela fica localizada” no ponto de início do labirinto. Podemos trocar de câmera Free para Look-At pelas teclas L e F.


- **Instâncias de objetos:**
Há a repetição de várias objetos em diferentes pontos do mapa, como os diamantes, os inimigos e os próprios blocos que compõem o labirinto. Para isso, foram utilizadas diferentes matrizes Modell.


- **Animações e Movimentações Baseadas no Tempo:**
Os diamantes e a arma (quando não capturadas) tem animação com movimento de rotação. Os inimigos se deslocam pelo mapa por meio de Curvas de Bézier. Já as movimentações do Golem e Escorpião de “subida/descida” , além da movimentação de “pulo” da personagem (e da arma que o acompanha) são feitas por meio de funções utilizando seno.
As animações de rotação e deslocamento de inimigos pelo mapa são controladas baseadas no tempo.


 - **Testes de Interseção Entre Objetos Virtuais:** Foram definidos 4 testes de interseção. Há a definição da colisão do personagem(câmera) com os inimigos e diamante ("player_enemy_collision"), que é **esfera-esfera** ; da colisão do personagem com o cenário do mapa ("cam_colisoes"), que é **esfera-plano** ; da colisão da bala com o cenário ("HitBullet"), que é **ponto-cubo** ; e da colisão da da bala com os inimigos ("HitBulletEnemy"), que é **ponto-esfera** . 



**Observação:** Não separamos as colisões em um arquivo "collisions.cpp", pois toda a lógica de colisão foi pensada antes de notarmos a necessidade desse requisito. E por receio de que uma alteração tão perto da entrega fosse comprometer o restante do jogo, optamos por não fazer essa separação. Das 4 funções de colisão feitas, 2 ficaram na main (HitBullet, HitBulletEnemy) e 2 no módulo player.h (player_enemy_collision, cam_colisoes)



# Manual de Uso

- Teclas W/A/S/D controlam a movimentação do personagem (e da arma que o acompanha, se estiver com a arma)
- Tecla Shift pressionada junto a alguma das teclas acima faz o personagem andar mais rápido
- Movimentação do mouse faz a câmera rotacionar
- Botão Esquerdo do Mouse faz o personagem atirar balas, se ele estiver com a arma
- Teclas F e L fazem a transformação para “Free Camera” e “Look At Camera”, respectivamente.




# Contribuições dos membros

Ambos os alunos acompanharam, sugeriram pontos e tópicos e discutiram em conjunto todas as etapas do projeto, incluindo a lógica e o funcionamento do jogo.
Mas, em sua maioria, cada um foi responsável pela implementação dos seguintes aspectos:

**Enzo** : 'Esqueleto" inicial do projeto, incluindo a definição dos módulos e a incorporação de funções chave da biblioteca OpenGl ;
            Definição de câmeras (free, look-at) ;
            Escolha e carregamento dos modelos obj. que representam os objetos do jogo ;
            Escolha e definição do mapeamento das texturas ;
            Definição dos modelos de iluminação (lambert, blinn-phong) ;
            Definição dos modelos de interpolação (phong, gouraud) ;
             Movimento de rotação inicial da arma.


**Geancarlo** : Definição e carregamento do mapa do jogo ;
                Definição do sistema de tiro ;
                Definição dos testes de colisão (jogador-inimigo , jogador-blocosMapa, bala-blocosMapa, bala-Inimigos) ;
                Definição dos movimentos de animação dos inimigos, baseados no tempo, e feitos a partir de Curvas de Bezier ;
                Escolha de texturas ;
                Instanciação de objetos (repetição dos blocos do mapa, e inimigos no mapa) ;
                Transformação geométrica controlada pelo usuário (arma acompanhando câmera).


# Utilização ChatGpt

Tentou-se utilizar o ChatGpt para  auxiliar na definição da mira para utilização no sistema da arma. Porém, não foi possível fazer com que a resposta se encaixasse
no código escrito até então, então o código foi feito por conta própria.



# Imagens do jogo


![Captura de tela 2024-02-09 163325](https://github.com/Enzosbx/TrabalhoFinalFCG/assets/91389519/1cd401f3-47c6-415b-ab27-c98898611f82)






