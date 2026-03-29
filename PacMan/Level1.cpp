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
#include "Home.h"
#include "Level1.h"
#include "Level2.h"
#include "Player.h"
#include "Wall.h"
#include "Pivot.h"
#include <string>
#include <fstream>
#include "Physics.h"
#include "Ghost.h"


using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void Level1::Init()
{
    Physics::Setup(800.0f);

    entities.clear();
    // 1. Inicializa o gerenciador de cena e fundo
    scene = new Scene();
    backg = new Sprite("Resources/Level1.jpg");

    // 2. CRIAÇÃO DO PLAYER
    // É vital que o Player seja o PRIMEIRO no vetor (índice 0)
    Player* player = new Player();
    entities.push_back(player); // Adiciona ao seu vetor de controle
    scene->Add(player, MOVING); // Adiciona à engine

    // 3. CRIAÇÃO DOS FANTASMAS INICIAIS
    // Vamos começar com 2 fantasmas (o Spawner no Update completará até o MAX_ENTITIES)
    for (int i = 0; i < 2; i++) {
        Ghost* redGhost = new Ghost(player);
        // Define posições diferentes para não nascerem um em cima do outro
        redGhost->MoveTo(100.0f + (i * 50.0f), 100.0f);

        entities.push_back(redGhost);
        scene->Add(redGhost, MOVING);
    }

    // 4. Paredes e Cenário
    /*Wall* w1 = new Wall(200.0f, 650.0f, 300.0f, 40.0f, "Resources/PacManL.png");
    scene->Add(w1, STATIC);

    Wall* w2 = new Wall(600.0f, 500.0f, 300.0f, 40.0f, "Resources/PacManL.png");
    scene->Add(w2, STATIC);*/

    // 5. Carregamento de Pivôs (Mantido)
    ifstream fin;
    fin.open("PivotsL1.txt");
    if (fin.is_open()) {
        bool left, right, up, down;
        float posX, posY;
        while (fin >> left >> right >> up >> down >> posX >> posY) {
            Pivot* pivot = new Pivot(left, right, up, down);
            pivot->MoveTo(posX, posY);
            scene->Add(pivot, STATIC);
        }
        fin.close();
    }
}

// ------------------------------------------------------------------------------

void Level1::Finalize()
{
    entities.clear();

    delete backg;
    delete scene;
}

// ------------------------------------------------------------------------------

void Level1::Update()
{

    scene->Update();
    scene->CollisionDetection();

    for (auto it = entities.begin(); it != entities.end(); ) {
        if (!(*it)->alive) {
            scene->Remove((*it), MOVING); // Remove da lógica de render/colisão da Scene
            delete (*it);                 // Libera memória
            it = entities.erase(it);      // Remove do seu vetor de controle
        }
        else {
            it++;
        }
    }

    if (entities.size() < MAX_ENTITIES) {
        // Pega a referência do player para a IA do fantasma
        Player* p = (Player*)entities[0];

        Ghost* newGhost = new Ghost(p);

        // Define uma posição de nascimento aleatória ou fixa
        newGhost->MoveTo(100.0f, 100.0f);

        entities.push_back(newGhost);
        scene->Add(newGhost, MOVING);
    }

    // habilita/desabilita bounding box
    if (window->KeyPress('B'))
    {
        viewBBox = !viewBBox;
    }

    if (window->KeyPress(VK_ESCAPE))
    {
        // volta para a tela de abertura
        Engine::Next<Home>();
    }
    else if (window->KeyPress('N'))
    {
        // passa manualmente para o próximo nível
        Engine::Next<Level2>();
    }
}

// ------------------------------------------------------------------------------

void Level1::Draw()
{
    // desenha cena
    backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    scene->Draw();

    // desenha bounding box dos objetos
    if (viewBBox)
        scene->DrawBBox();
}

// ------------------------------------------------------------------------------