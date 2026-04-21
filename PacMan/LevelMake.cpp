/**********************************************************************************
// LevelMake (Código Fonte)
//
// Criação:     18 Jan 2013
// Atualização: 21 Apr 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível base do jogo PacMan — inimigos e comidas por estágio
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

void LevelMake::LoadLevel(std::string path)
{
    std::ifstream file(path);
    std::string   line, key;

    if (!file.is_open()) return;

    std::vector<std::vector<PortalData>> tempPortals;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ss >> key;

        if (key == "COUNT")
        {
            ss >> bgCount;
            if (stages) delete[] stages;
            stages = new StageConfig[bgCount];
            tempPortals.resize(bgCount);
        }
        else if (key == "STAGE")
        {
            int i;
            std::string spritePath;
            float sx, sy;
            ss >> i >> spritePath >> sx >> sy;

            if (i < bgCount)
            {
                stages[i].background = new Sprite(spritePath);
                stages[i].spawnX = sx;
                stages[i].spawnY = sy;
                stages[i].portalCount = 0;
            }
        }
        else if (key == "PORTAL")
        {
            int i, target;
            float px, py;
            ss >> i >> px >> py >> target;

            if (i < bgCount)
                tempPortals[i].push_back({ px, py, target });
        }
    }

    for (int i = 0; i < bgCount; i++)
    {
        stages[i].portalCount = (int)tempPortals[i].size();
        stages[i].portals = new PortalData[stages[i].portalCount];
        for (int j = 0; j < stages[i].portalCount; j++)
            stages[i].portals[j] = tempPortals[i][j];
    }

    file.close();
}

// ------------------------------------------------------------------------------

void LevelMake::Init(float gravity, int maxFood, int maxGhost, string levelBackground)
{
    if (scene) { delete scene;  scene = nullptr; }
    if (player) { delete player; player = nullptr; }
    if (backg) { delete backg;  backg = nullptr; }

    Physics::Setup(gravity);
    MAX_GHOSTS = maxGhost;
    ghostAlive = MAX_GHOSTS;

    scene = new Scene();

    if (!levelBackground.empty()) {
        backg = new Sprite(levelBackground);
    }
    player = new Player();
    scene->Add(player, MOVING);
}

// ------------------------------------------------------------------------------

void LevelMake::Finalize()
{
    // -----------------------------------------------------------------------
    // REGRA: A Scene deleta tudo que foi adicionado via Add().
    //        Aqui apenas liberamos os arrays de rastreamento e metadados.
    //        NUNCA chamamos delete em Ghost*, Food* ou Portal* diretamente.
    // -----------------------------------------------------------------------

    if (stages != nullptr)
    {
        for (int i = 0; i < bgCount; i++)
        {
            // Sprite não foi adicionado à Scene — deletamos normalmente
            delete stages[i].background;
            stages[i].background = nullptr;

            // Dados de configuração de portais (structs, não objetos de cena)
            delete[] stages[i].portals;
            stages[i].portals = nullptr;
        }

        delete[] stages;
        stages = nullptr;
    }

    // O array activePortals rastreia objetos Portal* que estão na Scene.
    // A Scene os deleta — apenas liberamos o array.
    delete[] activePortals;
    activePortals = nullptr;
    activePortalCount = 0;

    // A Scene deleta player e todos os outros objetos registrados
    delete scene;
    scene = nullptr;

    // backg e consolas nunca foram adicionados à Scene
    delete backg;
    backg = nullptr;

    delete consolas;
    consolas = nullptr;
}

// ------------------------------------------------------------------------------

void LevelMake::Update()
{
    scene->Update();
    scene->CollisionDetection();
    UpdateStageTransition(gameTime);

    // Se a sala ainda não foi limpa, verificamos a existência de inimigos na Scene
    if (stages != nullptr && !stages[currentBG].visited)
    {
        
        if ( ghostAlive == 0)
        {
            stages[currentBG].visited = true;
            CreatePortalsForCurrentStage();
        }
    }

    if (window->KeyPress('B')) viewBBox = !viewBBox;

    if (player->GetHp() <= 0) {
        Engine::Next<Home>();
    }
}

// ------------------------------------------------------------------------------

void LevelMake::Draw()
{
    // 1. Desenha o Fundo
    if (stages != nullptr && stages[currentBG].background != nullptr)
        stages[currentBG].background->Draw(window->CenterX(), window->CenterY(), Layer::BACK);

    // 2. Desenha os Objetos da Cena (Player, Ghosts, Walls)
    scene->Draw();

    // 3. Interface de Texto (HUD)
    if (player != nullptr && consolas != nullptr)
    {
        // Cor dinâmica para o HP
        Color hpColor = (player->GetHp() < player->GetMaxHp() * 0.3f)
            ? Color(1.0f, 0.2f, 0.2f, 1.0f) // Vermelho se baixo
            : Color(1.0f, 1.0f, 1.0f, 1.0f);

        // HP do Jogador
        std::string hpStr = "HP: " + std::to_string((int)player->GetHp());
        consolas->Draw(40, 40, hpStr, hpColor);

        // Contador de Inimigos Restantes
        std::string ghostStr = "Inimigos: " + std::to_string(ghostAlive);
        consolas->Draw(40, 65, ghostStr, Color(1.0f, 1.0f, 0.4f, 1.0f));

        // Índice do Estágio Atual
        std::string stageStr = "Estagio: " + std::to_string(currentBG + 1);
        consolas->Draw(window->Width() - 150, 40, stageStr, Color(0.8f, 0.8f, 1.0f, 1.0f));

        // Status da Sala (Se os portais estão abertos)
        if (stages != nullptr) {
            if (stages[currentBG].visited) {
                consolas->Draw(window->CenterX() - 60, 40, "PORTAIS ABERTOS", Color(0.2f, 1.0f, 0.2f, 1.0f));
            }
            else {
                consolas->Draw(window->CenterX() - 70, 40, "DERROTE OS FANTASMAS", Color(1.0f, 0.5f, 0.0f, 1.0f));
            }
        }
    }

    // 4. Debug: Caixas de Colisão
    if (viewBBox)
        scene->DrawBBox();
}

// ------------------------------------------------------------------------------
// ghostInit / foodInit
//
// Cria os objetos e os adiciona à Scene via Add().
// A Scene passa a ser DONA da memória desses objetos.
// StageConfig apenas guarda ponteiros para poder fazer Remove/Add na troca de stage.
// ------------------------------------------------------------------------------

void LevelMake::ghostInit(int stageIndex)
{
    
    for (int i = 0; i < MAX_GHOSTS; i++)
    {
        addGhost(stageIndex);
    }
    

}

Ghost* LevelMake::addGhost(int stageIndex) {
    Ghost* ghost = new Ghost();
    ghost->SetTarget(player);
    if (stageIndex == currentBG) {
        scene->Add(ghost, MOVING);
    }
    return ghost;
}

Food* LevelMake::addFood(int stageIndex) {
    Food* food = new Food();

    if (stageIndex == currentBG) {
        scene->Add(food, STATIC);
    }
    return food;
}

// ------------------------------------------------------------------------------
// DetachStageEntities — Remove() sem deletar (Scene continua dona)
// AttachStageEntities — Add() para recolocar na cena
// ------------------------------------------------------------------------------


// ------------------------------------------------------------------------------

void LevelMake::SetStage(int index)
{
    if (index < 0 || index >= bgCount || stages == nullptr) return;

    // 1. Limpeza do estágio anterior (remove referências da Scene)


    // 2. Limpeza física dos portais ativos na Scene
    if (activePortals != nullptr)
    {
        for (int i = 0; i < activePortalCount; i++)
        {
            if (activePortals[i] != nullptr)
            {
                static_cast<Entity*>(activePortals[i])->SetAlive(false);
                scene->Delete(activePortals[i], STATIC);
            }
        }
        delete[] activePortals;
        activePortals = nullptr;
        activePortalCount = 0;
    }

    // 3. Atualiza o índice do estágio atual
    currentBG = index;

    // 4. Posiciona o player no novo Spawn
    if (player != nullptr)
        player->MoveTo(stages[currentBG].spawnX, stages[currentBG].spawnY);

    // 5. Lógica de Primeira Visita vs Sala Limpa
    if (!stages[currentBG].visited)
    {
        // Primeira vez: Criamos os fantasmas (as comidas nascerão na morte deles)
        ghostInit(currentBG);
		ghostAlive = MAX_GHOSTS;
    }
    else
    {
        // Já visitou e limpou: Apenas abre os portais para livre passagem
        CreatePortalsForCurrentStage();
    }
}

// ------------------------------------------------------------------------------

void LevelMake::ChangeBackground(int index)
{
    if (index >= 0 && index < bgCount)
        currentBG = index;
}

// ------------------------------------------------------------------------------

void LevelMake::GenerateMaze(Scene* scene, Window* window, int tileSize)
{
    int cols = window->Width() / tileSize;
    int rows = window->Height() / tileSize;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            bool createWall = false;

            if (r % 2 == 0 && c % 2 == 0)
            {
                createWall = true;

                int dir = rand() % 4;
                int wallR = r, wallC = c;
                if (dir == 0) wallR--;
                else if (dir == 1) wallR++;
                else if (dir == 2) wallC--;
                else               wallC++;

                bool isNearEdge = (wallR <= 0 || wallR >= rows - 1 || wallC <= 0 || wallC >= cols - 1);
                bool isCenter = (abs(wallC - cols / 2) <= 2 && abs(wallR - rows / 2) <= 2);

                if (!isNearEdge && !isCenter)
                {
                    float pConnX = wallC * tileSize + (tileSize / 2.0f);
                    float pConnY = wallR * tileSize + (tileSize / 2.0f);
                    scene->Add(new Wall(pConnX, pConnY, (float)tileSize, (float)tileSize, "Resources/Tijolo.png"), STATIC);
                }
            }

            if (r == 0 || r == rows - 1 || c == 0 || c == cols - 1) createWall = false;
            if (abs(c - cols / 2) <= 1 && abs(r - rows / 2) <= 1)   createWall = false;

            if (createWall)
            {
                float posX = c * tileSize + (tileSize / 2.0f);
                float posY = r * tileSize + (tileSize / 2.0f);
                scene->Add(new Wall(posX, posY, (float)tileSize, (float)tileSize, "Resources/Tijolo.png"), STATIC);
            }
        }
    }
}

// ------------------------------------------------------------------------------

float LevelMake::GetSpawnX(int index) const
{
    if (index < 0 || index >= bgCount) return 0.0f;
    return stages[index].spawnX;
}

float LevelMake::GetSpawnY(int index) const
{
    if (index < 0 || index >= bgCount) return 0.0f;
    return stages[index].spawnY;
}

void LevelMake::UpdateStageTransition(float dt)
{
    if (changingStage)
    {
        changeCooldown -= dt;
        if (changeCooldown <= 0.0f)
            changingStage = false;
    }
}

void LevelMake::CreatePortalsForCurrentStage()
{
    // Prevenção: não criar se já houver portais ativos
    if (activePortals != nullptr) return;

    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0)
    {
        activePortals = new Entity * [activePortalCount];
        for (int i = 0; i < activePortalCount; i++)
        {
            PortalData& data = stages[currentBG].portals[i];
            Portal* p = new Portal(data.x, data.y, data.targetBG);
            activePortals[i] = p;
            scene->Add(p, STATIC); // Adiciona fisicamente à Scene
        }
    }
}

// ------------------------------------------------------------------------------