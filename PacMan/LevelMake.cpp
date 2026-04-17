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
#include "Portal.h"


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
    // Limpeza da nova estrutura de Estágios
    if (stages != nullptr) {
        for (int i = 0; i < bgCount; i++) {
            // Deleta o sprite do background de cada cenário
            if (stages[i].background != nullptr) {
                delete stages[i].background;
            }
            // Deleta o array de dados de portais de cada cenário
            if (stages[i].portals != nullptr) {
                delete[] stages[i].portals;
            }
        }
        delete[] stages;
        stages = nullptr;
    }

    // Limpeza dos portais que estavam ativos na cena
    if (activePortals != nullptr) {
        delete[] activePortals;
        activePortals = nullptr;
    }

    delete scene;
    scene = nullptr;

    delete[] entities;
    delete[] foods;
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
    // Desenha o background do estágio atual
    if (stages != nullptr && stages[currentBG].background != nullptr)
    {
        stages[currentBG].background->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    }

    scene->Draw();

    if (viewBBox) {
        scene->DrawBBox();
    }
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

void LevelMake::ChangeBackground(int index) {
    if (index >= 0 && index < bgCount) {
        currentBG = index;
    }
}

void LevelMake::SetStage(int index) {
    if (index < 0 || index >= bgCount) return;

    // 1. Remover portais antigos da cena
    for (int i = 0; i < activePortalCount; i++) {
        scene->Remove(activePortals[i], STATIC);
    }
    delete[] activePortals;

    // 2. Atualizar o índice
    currentBG = index;

    // 3. Criar novos portais baseados na struct do cenário atual
    activePortalCount = stages[currentBG].portalCount;
    activePortals = new Entity * [activePortalCount];

    for (int i = 0; i < activePortalCount; i++) {
        PortalData data = stages[currentBG].portals[i];
        Portal* p = new Portal(data.x, data.y, data.targetBG);
        activePortals[i] = p;
        scene->Add(p, STATIC);
    }
}

// Accessor implementations for spawn points
float LevelMake::GetSpawnX(int index) const {
    if (index < 0 || index >= bgCount) return 0.0f;
    return stages[index].spawnX;
}

float LevelMake::GetSpawnY(int index) const {
    if (index < 0 || index >= bgCount) return 0.0f;
    return stages[index].spawnY;
}
// ------------------------------------------------------------------------------