/**********************************************************************************
// Level2 (Arquivo de Cabeçalho)
//
// Criação:     18 Jan 2013
// Atualização: 08 Abr 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível 2 do jogo PacMan (Padronizado com LevelMake)
//
**********************************************************************************/

#ifndef _PACMAN_LEVEL2_H_
#define _PACMAN_LEVEL2_H_

#include "LevelMake.h"
#include "Wall.h"
class Level2 : public LevelMake
{
private:
    Wall* testWall = nullptr;
public:
    void Init();                    // inicializa jogo
    void Update() override;         // atualiza lógica do jogo
};

#endif