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
#include "Food.h"


using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void Level1::Init()
{
    Physics::Setup(0.0f);

    playerAndGhost.clear();
    // 1. Inicializa o gerenciador de cena e fundo
    scene = new Scene();
    backg = new Sprite("Resources/Level1.jpg");

    // 2. CRIAÇÃO DO PLAYER
    // É vital que o Player seja o PRIMEIRO no vetor (índice 0)
    Player* player = new Player();
    playerAndGhost.push_back(player); // Adiciona ao seu vetor de controle
    scene->Add(player, MOVING); // Adiciona à engine

    // 3. CRIAÇÃO DOS FANTASMAS INICIAIS
    // Vamos começar com 2 fantasmas (o Spawner no Update completará até o MAX_ENTITIES)
    for (int i = 1; i < MAX_GHOSTS; i++) {
        Ghost* redGhost = new Ghost();
        // Define posições diferentes para não nascerem um em cima do outro
        redGhost->MoveTo(100.0f + (i * 50.0f), 100.0f);

        playerAndGhost.push_back(redGhost);
        scene->Add(redGhost, MOVING);
    }

    // 4. Paredes e Cenário
    /*Wall* w1 = new Wall(200.0f, 650.0f, 300.0f, 40.0f, "Resources/PacManL.png");
    scene->Add(w1, STATIC);

    Wall* w2 = new Wall(600.0f, 500.0f, 300.0f, 40.0f, "Resources/PacManL.png");
    scene->Add(w2, STATIC);*/

   

    for (int i = 0; i < MAX_FOOD; i++) {
        Food* food = new Food();
        food->MoveTo(100.0f + (i * 50.0f), 100.0f);

        foods.push_back(food);
        scene->Add(food, MOVING);
    }
}

// ------------------------------------------------------------------------------

void Level1::Finalize()
{

    foods.clear();
    playerAndGhost.clear();

    delete backg;
    delete scene;
}

// ------------------------------------------------------------------------------

void Level1::Update()
{

    scene->Update();
    scene->CollisionDetection();

    for (auto it = playerAndGhost.begin(); it != playerAndGhost.end(); ) {
        if (!(*it)->alive) {
            scene->Remove((*it), MOVING); // Remove da lógica de render/colisão da Scene
            delete (*it);                 // Libera memória
            it = playerAndGhost.erase(it);      // Remove do seu vetor de controle
        }
        else {
            it++;
        }
    }

    for (auto it = foods.begin(); it != foods.end(); ) {
        if (!(*it)->alive) {
            scene->Remove((*it), MOVING); // Remove da lógica de render/colisão da Scene
            delete (*it);                 // Libera memória
            it = foods.erase(it);      // Remove do seu vetor de controle
        }
        else {
            it++;
        }
    }

    if (foods.size() < MAX_FOOD) {

        Food* newFood = new Food();

        // Define uma posição de nascimento aleatória ou fixa
            newFood->MoveTo(100.0f, 100.0f);

            foods.push_back(newFood);
            scene->Add(newFood, MOVING);
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