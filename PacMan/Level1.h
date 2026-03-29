/**********************************************************************************
// Level1 (Arquivo de Cabeçalho)
// 
// Criação:     18 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível 1 do jogo PacMan
//
**********************************************************************************/

#ifndef _PACMAN_LEVEl1_H_
#define _PACMAN_LEVEL1_H_

// ------------------------------------------------------------------------------
// Inclusões

#include "Game.h"
#include "Sprite.h"
#include "Scene.h"
#include "Entity.h"

// ------------------------------------------------------------------------------

class Level1 : public Game
{
private:
    Sprite * backg = nullptr;       // background
    Scene * scene = nullptr;        // gerenciador de cena
    bool viewBBox = false;          // habilita visualização da bounding box

    const int MAX_ENTITIES = 15;
    std::vector<Entity*> entities;

public:
    void Init();                    // inicializa jogo
    void Update();                  // atualiza lógica do jogo
    void Draw();                    // desenha jogo
    void Finalize();                // finaliza jogo
};

// -----------------------------------------------------------------------------

#endif