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

#include "LevelMake.h"
#include "Home.h"

#include <fstream>
#include <cstdlib>

using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void LevelMake::Init()
{
    Physics::Setup(0);
    MAX_FOOD = 15;
    MAX_GHOSTS = 15;
    scene = new Scene();
    backg = new Sprite("Resources/fotoPadrao.png");
    entityCount = MAX_GHOSTS;
    foodCount = MAX_FOOD;

    entities = new Entity * [entityCount];
    foods = new Food * [foodCount];

    // Inicializa o Player na primeira posição
    Player* player = new Player();
    scene->Add(player, MOVING);
}

void LevelMake::Init(float gravity, int maxFood, int maxGhost, string levelBackground)
{
    Physics::Setup(gravity);
    MAX_FOOD = maxFood;
    MAX_GHOSTS = maxGhost;

    scene = new Scene();
    backg = new Sprite(levelBackground);

    // Alocação dinâmica dos vetores
    // entityCount é MAX_GHOSTS (fantasmas) + 1 (player)
    entityCount = MAX_GHOSTS;
    foodCount = MAX_FOOD;

    entities = new Entity * [entityCount];
    foods = new Food * [foodCount];

    // Inicializa o Player na primeira posição
    Player* player = new Player();
    scene->Add(player, MOVING);
}

// ------------------------------------------------------------------------------

void LevelMake::Finalize()
{
    delete scene;
    scene = nullptr;

    delete[] entities; // Deleta o array de ponteiros
    delete[] foods;

    delete backg;
    backg = nullptr;
}

// ------------------------------------------------------------------------------

void LevelMake::Update()
{

    scene->Update();
    scene->CollisionDetection();
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
}

// ------------------------------------------------------------------------------

void LevelMake::Draw()
{
    // desenha cena
    backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    scene->Draw();

    // desenha bounding box dos objetos
    if (viewBBox)
        scene->DrawBBox();
}

void LevelMake::ghostInit() {
    for (int i = 1; i < entityCount; i++) {
        Ghost* ghost = new Ghost();
        ghost->MoveTo(i * 40 + 50, i * 40 + 50);
        entities[i] = ghost; // Atribuição direta no vetor dinâmico
        scene->Add(ghost, MOVING);
    }
}

void LevelMake::foodInit()
{
    for (int i = 0; i < foodCount; i++) {
        Food* food = new Food();
        food->MoveTo(100.0f + (i * 50.0f), 100.0f);
        foods[i] = food; // Atribuição direta no vetor dinâmico
        scene->Add(food, STATIC);
    }
}

void LevelMake::GenerateMaze(Scene* scene, Window* window, int tileSize)
{
    int cols = window->Width() / tileSize;
    int rows = window->Height() / tileSize;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            bool createWall = false;

            // 1. Lógica de Pilares (Cria a estrutura básica do labirinto)
            if (r % 2 == 0 && c % 2 == 0) {
                createWall = true;

                // Tenta conectar o pilar a um vizinho para criar corredores
                int dir = rand() % 4;
                int wallR = r, wallC = c;
                if (dir == 0) wallR--;      // Cima
                else if (dir == 1) wallR++; // Baixo
                else if (dir == 2) wallC--; // Esquerda
                else if (dir == 3) wallC++; // Direita

                // Só cria a conexão se não for borda e não for o centro (spawn)
                bool isNearEdge = (wallR <= 0 || wallR >= rows - 1 || wallC <= 0 || wallC >= cols - 1);
                bool isCenter = (abs(wallC - cols / 2) <= 2 && abs(wallR - rows / 2) <= 2);

                if (!isNearEdge && !isCenter) {
                    float pConnX = wallC * tileSize + (tileSize / 2.0f);
                    float pConnY = wallR * tileSize + (tileSize / 2.0f);
                    scene->Add(new Wall(pConnX, pConnY, (float)tileSize, (float)tileSize, "Resources/Tijolo.png"), STATIC);
                }
            }

            // 2. SEGURANÇA: Garante que as bordas externas fiquem vazias (para o teleporte)
            if (r == 0 || r == rows - 1 || c == 0 || c == cols - 1) createWall = false;

            // Garante que o centro (onde o player nasce) esteja livre
            if (abs(c - cols / 2) <= 1 && abs(r - rows / 2) <= 1) createWall = false;

            if (createWall) {
                float posX = c * tileSize + (tileSize / 2.0f);
                float posY = r * tileSize + (tileSize / 2.0f);
                scene->Add(new Wall(posX, posY, (float)tileSize, (float)tileSize, "Resources/Tijolo.png"), STATIC);
            }
        }
    }
}

// ------------------------------------------------------------------------------