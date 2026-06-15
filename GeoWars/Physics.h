/**********************************************************************************
// Physics (Arquivo de Cabe�alho)
//
// Descri��o:   Sistema de f�sica com gravidade para plataformer
//
**********************************************************************************/

#ifndef PHYSICS_H
#define PHYSICS_H

// -------------------------------------------------------------------------------

class Physics
{
public:
    // Constantes de f�sica
    static constexpr float GRAVITY = 800.0f;   // pixels/s�  (realista)
    static constexpr float MAX_FALL_SPEED = 600.0f;   // velocidade m�xima de queda
    static constexpr float JUMP_FORCE = 720.0f;   // impulso do pulo
    static constexpr float DOUBLE_JUMP = 640.0f;   // impulso do segundo pulo
    static constexpr float MOVE_SPEED = 220.0f;   // velocidade horizontal
    static constexpr float DASH_SPEED = 620.0f;   // velocidade do dash
    static constexpr float DASH_DURATION = 0.18f;    // dura��o do dash em segundos
    static constexpr float DASH_COOLDOWN = 0.5f;     // cooldown do dash
    static constexpr float FRICTION = 0.82f;    // fric��o no ch�o
    static constexpr float AIR_RESISTANCE = 0.96f;    // resist�ncia do ar
    static constexpr float WALL_SLIDE_SPD = 80.0f;    // velocidade de deslizamento na parede
    static constexpr float COYOTE_TIME = 0.1f;     // tempo de coyote (pular ap�s sair da plataforma)
    static constexpr float JUMP_BUFFER = 0.12f;    // buffer de pulo antecipado
    static constexpr float MAX_SPEED = 250.0f;    // velocidade mxima
};

// -------------------------------------------------------------------------------

#endif
