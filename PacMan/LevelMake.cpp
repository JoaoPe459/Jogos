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
#include <sstream>
#include <cstdlib>

using std::ifstream;
using std::string;

// ------------------------------------------------------------------------------

void LevelMake::LoadLevel(std::string path) {
    std::ifstream file(path);
    std::string line, key;

    if (!file.is_open()) return;

    // Vetor temporário para organizar os portais antes de passar para o array fixo
    std::vector<std::vector<PortalData>> tempPortals;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ss >> key;

        if (key == "COUNT") {
            ss >> bgCount;
            if (stages) delete[] stages; // Limpa se já houver algo
            stages = new StageConfig[bgCount];
            tempPortals.resize(bgCount);
        }
        else if (key == "STAGE") {
            int i;
            std::string spritePath;
            float sx, sy;
            ss >> i >> spritePath >> sx >> sy;

            if (i < bgCount) {
                stages[i].background = new Sprite(spritePath);
                stages[i].spawnX = sx;
                stages[i].spawnY = sy;
                stages[i].portalCount = 0;
            }
        }
        else if (key == "PORTAL") {
            int i, target;
            float px, py;
            ss >> i >> px >> py >> target;

            if (i < bgCount) {
                tempPortals[i].push_back({ px, py, target });
            }
        }
    }

    // Transfere os portais dos vetores dinâmicos para a estrutura fixa da sua Engine
    for (int i = 0; i < bgCount; i++) {
        stages[i].portalCount = (int)tempPortals[i].size();
        stages[i].portals = new PortalData[stages[i].portalCount];
        for (int j = 0; j < stages[i].portalCount; j++) {
            stages[i].portals[j] = tempPortals[i][j];
        }
    }
    file.close();
}

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

    // Inicializa o Player de forma independente
    player = new Player();
    scene->Add(player, MOVING);

    // Inicializa apenas os fantasmas no array
    entityCount = MAX_GHOSTS;
    entities = new Entity * [entityCount];

    foodCount = MAX_FOOD;
    foods = new Food * [foodCount];
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
    UpdateStageTransition(gameTime);
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
    if (stages != nullptr && stages[currentBG].background != nullptr)
    {
        stages[currentBG].background->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    }

    scene->Draw();

    if (viewBBox) {
        scene->DrawBBox();
        for (int i = 0; i < activePortalCount; i++) {
            if (activePortals[i] != nullptr) { 
            }
        }
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
    if (index < 0 || index >= bgCount || stages == nullptr) return;

    // 1. Limpeza de portais antigos
    if (activePortals != nullptr) {
        for (int i = 0; i < activePortalCount; i++) {
            if (activePortals[i] != nullptr) scene->Remove(activePortals[i], STATIC);
        }
        delete[] activePortals;
        activePortals = nullptr;
    }

    currentBG = index;

    // 2. Teleporte Seguro do Player
    if (player != nullptr) {
        player->MoveTo(stages[currentBG].spawnX, stages[currentBG].spawnY);
    }

    // 3. Criar novos portais conforme o arquivo TXT
    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0) {
        activePortals = new Entity * [activePortalCount];
        for (int i = 0; i < activePortalCount; i++) {
            PortalData data = stages[currentBG].portals[i];
            Portal* p = new Portal(data.x, data.y, data.targetBG);
            activePortals[i] = p;
            scene->Add(p, STATIC);
        }
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

void LevelMake::UpdateStageTransition(float dt) {
    if (changingStage) {
        changeCooldown -= dt;
        if (changeCooldown <= 0.0f) {
            changingStage = false;
        }
    }
}
// ------------------------------------------------------------------------------