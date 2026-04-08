/**********************************************************************************
// Level1 (Código Fonte)
// Descrição: Nível com geração aleatória e teletransporte de bordas
**********************************************************************************/
#include "Engine.h"
#include "Home.h"
#include "Level1.h"
#include "Player.h"
#include "Wall.h"
#include "Ghost.h"
#include "Food.h"
#include "Physics.h"
#include <ctime>

// ------------------------------------------------------------------------------
void Level1::Init()
{
    srand(static_cast<unsigned int>(time(NULL)));
    Physics::Setup(0.0f);
    playerAndGhost.clear();
    foods.clear();

    scene = new Scene();
    backg = new Sprite("Resources/Level1.jpg");

    // Chama a função de labirinto
    int tileSize = 44;
    //GenerateMaze(scene, window, tileSize);

    // 2. CRIAÇÃO DO PLAYER
    Player* player = new Player();
    player->MoveTo(window->CenterX(), window->CenterY());
    playerAndGhost.push_back(player);
    scene->Add(player, MOVING);

    // 3. FANTASMAS (Spawnando em áreas vazias baseadas no grid)
    for (int i = 0; i < MAX_GHOSTS; i++) {
        Ghost* g = new Ghost();
        int rx = (rand() % (window->Width() / tileSize - 2)) + 1;
        int ry = (rand() % (window->Height() / tileSize - 2)) + 1;
        g->MoveTo(rx * tileSize + (tileSize / 2.0f), ry * tileSize + (tileSize / 2.0f));
        playerAndGhost.push_back(g);
        scene->Add(g, MOVING);
    }

    // 4. COMIDAS
    for (int i = 0; i < MAX_FOOD; i++) {
        Food* food = new Food();
        int rx = (rand() % (window->Width() / tileSize - 2)) + 1;
        int ry = (rand() % (window->Height() / tileSize - 2)) + 1;
        food->MoveTo(rx * tileSize + (tileSize / 2.0f), ry * tileSize + (tileSize / 2.0f));
        foods.push_back(food);
        scene->Add(food, MOVING);
    }
}

void Level1::GenerateMaze(Scene* scene, Window* window, int tileSize)
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
void Level1::Update()
{
    scene->Update();
    scene->CollisionDetection();

    // 5. LÓGICA DE TELETRANSPORTE (Wrap-around)
    // Aplica para o Player (index 0) e todos os Fantasmas
    for (auto const& entity : playerAndGhost) {
        float x = entity->X();
        float y = entity->Y();
        float offset = 20.0f; // Metade do tamanho aproximado do sprite

        // Atravessar Horizontalmente
        if (x + offset < 0)
            entity->MoveTo(window->Width() + offset, y);
        else if (x - offset > window->Width())
            entity->MoveTo(-offset, y);

        // Atravessar Verticalmente
        if (y + offset < 0)
            entity->MoveTo(x, window->Height() + offset);
        else if (y - offset > window->Height())
            entity->MoveTo(x, -offset);
    }

    if (window->KeyPress('B')) viewBBox = !viewBBox;
    if (window->KeyPress(VK_ESCAPE)) Engine::Next<Home>();
}

// ------------------------------------------------------------------------------
void Level1::Draw()
{
    backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    scene->Draw();

    if (viewBBox)
        scene->DrawBBox();
}

// ------------------------------------------------------------------------------
void Level1::Finalize()
{
    // Limpeza de vetores
    foods.clear();
    playerAndGhost.clear();

    delete backg;
    delete scene;
}