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

#include "LevelMake.h"
// ------------------------------------------------------------------------------

class Level1 : public LevelMake
{
private:
    
public:
    void Init();                    // inicializa jogo
    void Update() override;                  // atualiza lógica do jogo
    void Draw() override;                    // desenha jogo
    //void Finalize() override;                // finaliza jogo
};

// -----------------------------------------------------------------------------

#endif