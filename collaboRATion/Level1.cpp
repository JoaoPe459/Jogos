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
#include "LevelSelect.h"
#include "Pivot.h"
#include <string>
#include <fstream>
#include "Wall.h"
using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void Level1::Init()
{
    LevelMake::Init(600.0f, 0, 0, "Resources/Level2.jpg");
    LoadLevel2("Resources/inicio.txt");
}

// ------------------------------------------------------------------------------


// ------------------------------------------------------------------------------

void Level1::Update()
{
    LevelMake::Update();
    if (window->KeyPress('N')) {
        Engine::Next<Level2>();
    }
    if (window->KeyPress(VK_RETURN))
    {
		Engine::Next<LevelSelect>();
    }
}

void Level1::Draw()
{
    LevelMake::Draw();

    float posX = (1312.0f / 2.0f) - 300.0f;

    float posY = (768.0f / 4.0f) - 20.0f;

    terminal->Draw(posX, posY, "Aperte Enter para iniciar", Color(0.0f, 0.0f, 0.0f, 1.0f), Layer::FRONT, 1);

}

// ------------------------------------------------------------------------------
