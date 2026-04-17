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
    LevelMake::ghostInit();
    LevelMake::foodInit();

    int tileSize = 44;
    //GenerateMaze(scene, window, tileSize);
}

// ------------------------------------------------------------------------------

void Level2::Update()
{
    for (int i = 0; i < entityCount; i++) {
        if (entities[i] != nullptr && !entities[i]->IsAlive()) {
            entities[i]->MoveTo(window->CenterX(), window->CenterY());
        }
    }

    for (int i = 0; i < foodCount; i++) {
        if (foods[i] != nullptr && !foods[i]->IsAlive()) {
            foods[i]->MoveTo(200.0f + (rand() % 400), 200.0f + (rand() % 300));
        }
    }

    if (window->KeyPress(VK_ESCAPE)) {
        Engine::Next<Home>();
        return;
    }

    LevelMake::Update();
}

// ------------------------------------------------------------------------------