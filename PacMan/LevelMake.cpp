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
#include "PacMan.h"
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

void LevelMake::DrawCentralMessage(const std::string& text, Color color, float x, float y) {
    if (text.empty() || consolas == nullptr) return;

    // Se você passar x = -1, a função centraliza automaticamente no eixo X
    float finalX = x;
    if (x == -1.0f) {
        float textWidth = text.length() * 8.0f;
        finalX = window->CenterX() - (textWidth / 2.0f);
    }

    consolas->Draw(finalX, y, text, color);
}

void LevelMake::DrawHeartHealth()
{
    if (player == nullptr || heartSprite == nullptr) return;

    // Etapa 1: define quantos coracoes representam a vida maxima do jogador.
    const int heartCount = 10;
    const int maxHp = player->GetMaxHp();
    int hp = player->GetHp();

    if (hp < 0) hp = 0;

    // Etapa 2: converte o HP atual para uma quantidade proporcional de coracoes.
    int filledHearts = 0;
    if (maxHp > 0 && hp > 0)
        filledHearts = (hp * heartCount + maxHp - 1) / maxHp;

    if (filledHearts > heartCount)
        filledHearts = heartCount;

    // Etapa 3: centraliza a fileira de coracoes no topo do HUD.
    const float scale = 2.0f;
    const float spacing = 6.0f;
    const float heartWidth = heartSprite->Width() * scale;
    const float totalWidth = (heartWidth * heartCount) + (spacing * (heartCount - 1));
    const float startX = window->CenterX() - (totalWidth / 2.0f) + (heartWidth / 2.0f);
    const float y = 72.0f;

    for (int i = 0; i < heartCount; i++)
    {
        // Etapa 4: desenha coracao cheio para HP atual e apagado para HP perdido.
        const bool filled = i < filledHearts;
        const Color color = filled
            ? Color(1.0f, 1.0f, 1.0f, 1.0f)
            : Color(0.2f, 0.2f, 0.2f, 0.35f);

        heartSprite->Draw(startX + i * (heartWidth + spacing), y, Layer::FRONT, scale, 0.0f, color);
    }
}

// ------------------------------------------------------------------------------

void LevelMake::Init(float gravity, int maxFood, int maxGhost, string levelBackground)
{
    if (scene) { delete scene;  scene = nullptr; }
    if (player) { delete player; player = nullptr; }
    if (backg) { delete backg;  backg = nullptr; }
    if (heartSprite) { delete heartSprite; heartSprite = nullptr; }

    Physics::Setup(gravity);
    MAX_GHOSTS = maxGhost;
    ghostAlive = MAX_GHOSTS;

    scene = new Scene();

    if (!levelBackground.empty()) {
        backg = new Sprite(levelBackground);
    }
    // Carrega uma unica vez o sprite usado pelo HUD de vida.
    heartSprite = new Sprite("Resources/Heart.png");
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

    // backg, heartSprite e consolas nunca foram adicionados à Scene
    delete backg;
    backg = nullptr;

    delete heartSprite;
    heartSprite = nullptr;

    delete consolas;
    consolas = nullptr;
}

// ------------------------------------------------------------------------------

void LevelMake::Update()
{
    scene->Update();
    scene->CollisionDetection();
    UpdateStageTransition(gameTime);

    // Verifica se a sala ainda não foi concluída
    if (stages != nullptr && !stages[currentBG].visited) {

        // CONDIÇÃO 1: Inimigos mortos e comida ainda não spawnada
        // foodSpawned é uma variável booleana que você deve adicionar à classe
        if (ghostAlive == 0 && !foodSpawned)
        {
            addFood(currentBG);
            foodSpawned = true; // Garante que spawna apenas UMA comida
        }

        // CONDIÇÃO 2: Player consumiu o item
        if (comeuItem) {
            stages[currentBG].visited = true;
            CreatePortalsForCurrentStage();
            foodSpawned = false; // Reseta para a próxima sala
            comeuItem = false;   // Reseta o estado de consumo
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

    // 2. Desenha os Objetos da Cena
    scene->Draw();

    // 2. MENSAGEM DE HP (Logo abaixo da principal)
    if (player != nullptr && consolas != nullptr)
    {
        // 1. MENSAGEM DE STATUS (Centralizada no topo)
        if (ghostAlive > 0) {
            DrawCentralMessage("DERROTE OS INIMIGOS", Color(1.0f, 0.5f, 0.0f, 1.0f), -1.0f, 40.0f);
        }
        else if (!comeuItem) {
            DrawCentralMessage("ITEM DISPONIVEL! CONSUMA PARA SAIR", Color(0.4f, 0.8f, 1.0f, 1.0f), -1.0f, 40.0f);
        }
        else {
            DrawCentralMessage("PORTAS ABERTAS!", Color(0.2f, 1.0f, 0.2f, 1.0f), -1.0f, 40.0f);
        }

        // 2. VIDA DO JOGADOR (em coracoes)
        DrawHeartHealth();

        // 3. ESTÁGIO (No canto inferior direito ou centralizado abaixo)
        std::string stageStr = "ESTAGIO: " + std::to_string(currentBG + 1);
        DrawCentralMessage(stageStr, Color(0.8f, 0.8f, 1.0f, 1.0f), -1.0f, 112.0f);
    }

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
        addSnowman(stageIndex);
    }
    

}

Enemy* LevelMake::addSnowman(int stageIndex) {
    Enemy* snowman = new Enemy();
    snowman->SetTarget(player);

    if (stageIndex == currentBG) {
        scene->Add(snowman, MOVING);
    }
    return snowman;
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


    // 2. Limpeza física dos portais/portas ativos na Scene
    ClearActivePortals();

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
        CreateClosedDoorsForCurrentStage();
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
    ClearActivePortals();

    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0)
    {
        activePortals = new Entity * [activePortalCount];
        for (int i = 0; i < activePortalCount; i++)
        {
            PortalData& data = stages[currentBG].portals[i];
            // Converte a posicao antiga da borda da janela para a borda real do chao.
            Portal* p = new Portal(PortalX(data.x), PortalY(data.y), data.targetBG, true, PortalRotation(data.x, data.y));
            activePortals[i] = p;
            scene->Add(p, STATIC); // Adiciona fisicamente à Scene
        }
    }
}

void LevelMake::CreateClosedDoorsForCurrentStage()
{
    ClearActivePortals();

    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0)
    {
        activePortals = new Entity * [activePortalCount];
        for (int i = 0; i < activePortalCount; i++)
        {
            PortalData& data = stages[currentBG].portals[i];
            // A porta fechada fica visualmente no mesmo ponto do portal aberto.
            Portal* p = new Portal(PortalX(data.x), PortalY(data.y), data.targetBG, false, PortalRotation(data.x, data.y));
            activePortals[i] = p;
            scene->Add(p, STATIC);
        }
    }
}

void LevelMake::ClearActivePortals()
{
    if (activePortals == nullptr) return;

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

float LevelMake::PortalRotation(float x, float y) const
{
    const float pi = 3.14159265f;

    if (y < 100.0f)
        return 0.0f;
    if (y > window->Height() - 100.0f)
        return pi;
    if (x < 100.0f)
        return -pi / 2.0f;
    if (x > window->Width() - 100.0f)
        return pi / 2.0f;

    return 0.0f;
}

float LevelMake::PortalX(float x) const
{
    // Portais laterais eram definidos fora do chao; reposiciona para a borda jogavel.
    if (x < 100.0f)
        return 150;
    if (x > window->Width() - 100.0f)
        return 1150;

    return x;
}

float LevelMake::PortalY(float y) const
{
    // Portais de cima/baixo seguem a borda interna do piso do background.
    if (y < 100.0f)
        return 100;
    if (y > window->Height() - 100.0f)
        return 690;

    return y;
}

// ------------------------------------------------------------------------------
