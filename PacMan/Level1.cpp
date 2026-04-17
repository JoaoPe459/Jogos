/**********************************************************************************
// Level1 (Código Fonte) 
// 
// Criação:     18 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível 1 do jogo PacMan
//
**********************************************************************************/

#include "Engine.h"
#include "Level1.h"
#include "Level2.h"
#include "Pivot.h"
#include <string>
#include <fstream>

using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void Level1::Init() {
    // Chama o organizador que agora está no LevelMake
    LoadLevel("Resources/Levels/Level1.txt");

    // Inicializa o restante da lógica (fantasmas, comida, etc)
    LevelMake::Init(0.0f, 10, 10, "");
    ghostInit();
    foodInit();
    SetStage(0);
}

// ------------------------------------------------------------------------------


// ------------------------------------------------------------------------------

void Level1::Update()
{
    

    if (window->KeyPress('N')) {
        Engine::Next<Level2>();
        return;
    }

    LevelMake::Update();
}

// ------------------------------------------------------------------------------
