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
    bgCount = 8;
    stages = new StageConfig[bgCount];

    // Nomes dos arquivos conforme carregados anteriormente
    string files[] = {
        "Resources/Background1.png",   // 0
        "Resources/Background2.png",   // 1
        "Resources/Background2.1.png", // 2
        "Resources/Background2.2.png", // 3
        "Resources/Background2.3.png", // 4
        "Resources/Background3.png",   // 5
        "Resources/Background3.1.png", // 6
        "Resources/Background3.2.png"  // 7
    };

    // Semente para o random (importante para não ser sempre o mesmo padrão)
    srand(static_cast<unsigned int>(time(NULL)));

    for (int i = 0; i < bgCount; i++) {
        stages[i].background = new Sprite(files[i]);
        stages[i].portalCount = 2;
        stages[i].portals = new PortalData[2];

        // Configuração Circular dos Portais
        int prev = (i == 0) ? bgCount - 1 : i - 1;
        stages[i].portals[0] = { 30.0f, (float)window->CenterY(), prev };

        int next = (i == bgCount - 1) ? 0 : i + 1;
        stages[i].portals[1] = { (float)window->Width() - 30.0f, (float)window->CenterY(), next };

        // TELEPORTE RANDOM: Define uma posição aleatória dentro dos limites da janela
        // Deixamos uma margem de 100px para não nascer colado na borda
        stages[i].spawnX = (float)(rand() % (window->Width() - 200) + 100);
        stages[i].spawnY = (float)(rand() % (window->Height() - 200) + 100);
    }

    // Inicializa lógica base do LevelMake
    LevelMake::Init(0, 0, 0, "");

    // Define o estágio inicial
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
