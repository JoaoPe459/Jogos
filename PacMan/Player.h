/**********************************************************************************
// Player (Arquivo de Cabeçalho)
// 
// Criação:     01 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Player do jogo PacMan
//
**********************************************************************************/

#ifndef _PACMAN_PLAYER_H_
#define _PACMAN_PLAYER_H_

// ---------------------------------------------------------------------------------
// Inclusões

#include "Types.h"                      // tipos específicos da engine
#include "Object.h"                     // interface de Object
#include "Sprite.h"                     // interface de Sprites
#include "Moves.h"

// ---------------------------------------------------------------------------------
// Constantes Globais

// estados possíveis para o jogador
enum PLAYERSTATE {STOPED, UP, DOWN, LEFT, RIGHT};    

// ---------------------------------------------------------------------------------

class Player : public Object
{
private:
    Sprite * spriteL = nullptr;         // sprite do player indo para esquerda
    Sprite * spriteR = nullptr;         // sprite do player indo para direita
    Sprite * spriteU = nullptr;         // sprite do player indo para cima
    Sprite * spriteD = nullptr;         // sprite do player indo para baixo  
	Moves* moves = nullptr;


public:
    uint currState = STOPED;            // estado atual do jogador
    uint nextState = STOPED;            // próximo estado do jogador

    Player();                           // construtor
    ~Player();                          // destrutor

    void OnCollision(Object * obj);     // resolução da colisão

    void Update();                      // atualização do objeto
    
    void Draw();                        // desenho do objeto

    void HandleInput();

    void ApplyMovement();

    void ScreenWrap();

    void incrementSpeed();

    void decressSpeed();


};

// ---------------------------------------------------------------------------------

#endif