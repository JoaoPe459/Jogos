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

void Level1::Init()
{
    LevelMake::Init(600, 10, 10, "Resources/Level1.jpg");
    LevelMake::ghostInit();
    LevelMake::foodInit();
	//int tileSize = 44;
    //GenerateMaze(scene, window, tileSize);
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
