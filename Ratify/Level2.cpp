/**********************************************************************************
// Level2 (Código Fonte)
//
// Criação:     18 Jan 2013
// Atualização: 08 Abr 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível 2 do jogo PacMan
//
**********************************************************************************/

#include "Engine.h"
#include "Home.h"
#include "Level2.h"
#include "Pivot.h"
#include <string>

// ------------------------------------------------------------------------------

void Level2::Init()
{
    LevelMake::Init(600, 5, 5, "Resources/Level2.jpg");

    int tileSize = 44;
    //GenerateMaze(scene, window, tileSize);
}

// ------------------------------------------------------------------------------

void Level2::Update()
{

    if (window->KeyPress(VK_ESCAPE)) {
        Engine::Next<Home>();
        return;
    }

    LevelMake::Update();
}

// ------------------------------------------------------------------------------