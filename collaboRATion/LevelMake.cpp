/**********************************************************************************
// LevelMake (Código Fonte)
//
// Criação:     18 Jan 2013
// Atualização: 28 Mai 2026
// Compilador:  Visual C++ 2022
**********************************************************************************/

#include "LevelMake.h"
#include "Home.h"
#include "PacMan.h"
#include "Portal.h"
#include "LevelSelect.h"
#include "Interactables.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>

using std::ifstream;
using std::string;

// ─────────────────────────────────────────────────────────────────────────────
// LoadLevel — parser original (portais e stages)
// ─────────────────────────────────────────────────────────────────────────────

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
                stages[i].spawnX     = sx;
                stages[i].spawnY     = sy;
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
        stages[i].portals     = new PortalData[stages[i].portalCount];
        for (int j = 0; j < stages[i].portalCount; j++)
            stages[i].portals[j] = tempPortals[i][j];
    }

    file.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// LoadLevel2 — parser expandido estilo Level Devil
//
// Tokens suportados:
//
//   # comentário
//   GRAVITY   <g>
//
//   WALL      <id>  <x>  <y>
//   BLOCK     <wallId>  <offX> <offY>  <w> <h>  <sprite>  <tag>
//   SCALE     <wallId>  <sx>  [sy]          (sy omitido = proporcional)
//   ROTATE    <wallId>  <angle>
//
//   KILLZONE  <id>  <x>  <y>  <w>  <h>  <lethal:0|1>  [tag]
//
//   SPIKE     <wallId>  <x>  <y>  <interval>  <duration>  <randRange>
//   PLATFORM  <wallId>  <baseX>  <baseY>  <speed>  <range>
//   SPINNER   <wallId>  <speed>
//   FALLING   <wallId>  <delay>
//   HAZARD    <kzId>    <radius>  <damage>  <interval>
//   COUNTDOWN <seconds>
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::LoadLevel2(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line, key;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ss >> key;

        // ── Gravidade ────────────────────────────────────────────
        if (key == "GRAVITY")
        {
            float g;
            ss >> g;    
            Physics::Setup(g);
        }

        // ── Wall base ────────────────────────────────────────────
        else if (key == "WALL")
        {
            std::string id;
            float wx, wy;
            ss >> id >> wx >> wy;

            Wall* w = new Wall();
            w->MoveTo(wx, wy);  
            scene->Add(w, STATIC);
            parsedWalls[id] = w;
        }

        else if (key == "BOLA") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Wall* w = new Wall(); w->MoveTo(wx, wy);
            w->AddBlock(0, 0, 32, 32, "Resources/Bola.png", id);
            scene->Add(w, STATIC);
            parsedWalls[id] = w;
        }
        else if (key == "ESPINHO") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Spike* spike = new Spike(wx, wy, "Resources/Espinhos.png");
            scene->Add(spike, STATIC);
        }
        else if (key == "BOTAO") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            ButtonObj* btn = new ButtonObj(wx, wy, id);
            scene->Add(btn, STATIC);
        }
        else if (key == "PORTA") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Door* door = new Door(wx, wy, "Resources/Porta.png");
            scene->Add(door, STATIC);
        }

        // Spawn
        
        else if (key == "SPAWN")
        {
            float x, y;
            ss >> x >> y;

            if (player)
                player->MoveTo(x, y);
        }

        else if (key == "DECO_BLOCK") {
            float wx, wy;
            ss >> wx >> wy;

            DecoObj* d = new DecoObj(wx, wy, "Resources/Tijolo.png");
            scene->Add(d, STATIC);
        }
        else if (key == "DECO_RECT2") {
            float wx, wy;
            ss >> wx >> wy;

            for (int i = 0; i < 6; i++) {
                DecoObj* d = new DecoObj(wx + (i * 32.0f), wy, "Resources/Tijolo.png");
                scene->Add(d, STATIC);
            }
        }


        // ── Bloco vinculado a uma Wall ────────────────────────────
        else if (key == "BLOCK")
        {
            std::string wallId, sprite, tag;
            float offX, offY, bw, bh;
            ss >> wallId >> offX >> offY >> bw >> bh >> sprite >> tag;

            auto it = parsedWalls.find(wallId);
            if (it != parsedWalls.end())
                it->second->AddBlock(offX, offY, bw, bh, sprite.c_str(), tag);
        }
        // ── Retângulos agrupados ───────────────────────────
        else if (key == "RECT1" || key == "RECT2" || key == "RECT3" ||
            key == "RECT1V" || key == "RECT2V" || key == "RECT3V")
        {
            std::string wallId, sprite, tag;
            float offX, offY, bw, bh;
            ss >> wallId >> offX >> offY >> bw >> bh >> sprite >> tag;

            auto it = parsedWalls.find(wallId);
            if (it != parsedWalls.end())
            {
                if (key == "RECT1")       it->second->AddRect1(offX, offY, bw, bh, sprite.c_str(), tag);
                else if (key == "RECT2")  it->second->AddRect2(offX, offY, bw, bh, sprite.c_str(), tag);
                else if (key == "RECT3")  it->second->AddRect3(offX, offY, bw, bh, sprite.c_str(), tag);
                else if (key == "RECT1V") it->second->AddRect1v(offX, offY, bw, bh, sprite.c_str(), tag);
                else if (key == "RECT2V") it->second->AddRect2v(offX, offY, bw, bh, sprite.c_str(), tag);
                else if (key == "RECT3V") it->second->AddRect3v(offX, offY, bw, bh, sprite.c_str(), tag);
            }
        }

        // ── Escala de Wall ────────────────────────────────────────
        else if (key == "SCALE")
        {
            std::string wallId;
            float sx, sy;
            ss >> wallId >> sx;

            auto it = parsedWalls.find(wallId);
            if (it != parsedWalls.end())
            {
                // sy é opcional — se ausente usa sx (proporcional)
                if (ss >> sy)
                    it->second->SetScale(sx, sy);
                else
                    it->second->SetScale(sx);
            }
        }

        // ── Rotação inicial de Wall ───────────────────────────────
        else if (key == "ROTATE")
        {
            std::string wallId;
            float ang;
            ss >> wallId >> ang;

            auto it = parsedWalls.find(wallId);
            if (it != parsedWalls.end())
                it->second->SetAngle(ang);
        }

        // ── KillZone ──────────────────────────────────────────────
        else if (key == "KILLZONE")
        {
            std::string id, tag;
            float kx, ky, kw, kh;
            int   lethalInt;
            ss >> id >> kx >> ky >> kw >> kh >> lethalInt;
            ss >> tag; // opcional

            KillZone* kz = new KillZone(kx, ky, kw, kh,
                                         lethalInt != 0, tag);
            scene->Add(kz, STATIC);
            parsedKillZones[id] = kz;
        }

        // ── Espinhos dinâmicos ────────────────────────────────────
        else if (key == "SPIKE")
        {
            SpikeData sd;
            ss >> sd.wallId >> sd.x >> sd.y
               >> sd.interval >> sd.duration >> sd.randRange;
            parsedSpikes.push_back(sd);
            spikeTimers.push_back(0.0f);
            spikeAliveTimers.push_back(0.0f);
            spikeActive.push_back(false);
        }

        // ── Plataforma ping-pong ──────────────────────────────────
        else if (key == "PLATFORM")
        {
            PlatformData pd;
            ss >> pd.wallId >> pd.baseX >> pd.baseY
               >> pd.speed >> pd.range;
            parsedPlatforms.push_back(pd);
            platformPhases.push_back(0.0f);
        }

        // ── Parede giratória ──────────────────────────────────────
        else if (key == "SPINNER")
        {
            SpinnerData sd;
            ss >> sd.wallId >> sd.speed;
            parsedSpinners.push_back(sd);
        }

        // ── Parede que cai ────────────────────────────────────────
        else if (key == "FALLING")
        {
            FallingData fd;
            ss >> fd.wallId >> fd.delay;
            parsedFalling.push_back(fd);
            fallingTimers.push_back(0.0f);
            fallingDone.push_back(false);
        }

        // ── Hazard zone (drena HP por tick via KillZone) ──────────
        else if (key == "HAZARD")
        {
            HazardData hd;
            ss >> hd.kzId >> hd.radius >> hd.damage >> hd.interval;

            // Configura a KillZone existente como hazard (não letal, com tick)
            auto it = parsedKillZones.find(hd.kzId);
            if (it != parsedKillZones.end())
            {
                it->second->lethal       = false;
                it->second->damage       = hd.damage;
                it->second->tickInterval = hd.interval;
            }
            parsedHazards.push_back(hd);
        }

        // ── Countdown de morte ────────────────────────────────────
        else if (key == "COUNTDOWN")
        {
            ss >> parsedCountdown.seconds;
            parsedCountdown.active = true;
        }
    }

    file.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// UpdateParsedMechanics — chamado em Update() para animar tudo do parser
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::UpdateParsedMechanics()
{
    // ── 1. Espinhos ───────────────────────────────────────────────
    for (int i = 0; i < (int)parsedSpikes.size(); i++)
    {
        SpikeData& sd = parsedSpikes[i];
        auto it = parsedWalls.find(sd.wallId);
        if (it == parsedWalls.end()) continue;
        Wall* w = it->second;

        if (!spikeActive[i])
        {
            spikeTimers[i] += gameTime;
            if (spikeTimers[i] >= sd.interval)
            {
                spikeTimers[i]      = 0.0f;
                spikeActive[i]      = true;
                spikeAliveTimers[i] = 0.0f;

                float randX = sd.x + (float)(rand() % (int)sd.randRange)
                              - sd.randRange * 0.5f;
                w->MoveTo(randX, sd.y);
                w->AddBlock(0, 0, 36, 36, "Resources/Tijolo.png", "spike_dyn");
            }
        }
        else
        {
            spikeAliveTimers[i] += gameTime;

            // Remove por proximidade do player
            w->RemoveBlocksInRadius(player->X(), player->Y(), 50.0f);

            // Remove por expiração
            if (spikeAliveTimers[i] >= sd.duration)
            {
                w->RemoveBlock("spike_dyn");
                spikeActive[i] = false;
            }
        }
    }

    // ── 2. Plataformas ping-pong ──────────────────────────────────
    for (int i = 0; i < (int)parsedPlatforms.size(); i++)
    {
        PlatformData& pd = parsedPlatforms[i];
        auto it = parsedWalls.find(pd.wallId);
        if (it == parsedWalls.end()) continue;
        Wall* w = it->second;

        platformPhases[i] += pd.speed * gameTime;
        float newX = pd.baseX + sinf(platformPhases[i]) * pd.range;
        w->MoveTo(newX, pd.baseY);

        for (Block* b : w->GetBlocks())
            b->SyncToOwner();
    }

    // ── 3. Spinners ───────────────────────────────────────────────
    for (auto& sd : parsedSpinners)
    {
        auto it = parsedWalls.find(sd.wallId);
        if (it == parsedWalls.end()) continue;
        it->second->Rotate(sd.speed * gameTime);
    }

    // ── 4. Paredes que caem ───────────────────────────────────────
    for (int i = 0; i < (int)parsedFalling.size(); i++)
    {
        if (fallingDone[i]) continue;

        FallingData& fd = parsedFalling[i];
        auto it = parsedWalls.find(fd.wallId);
        if (it == parsedWalls.end()) continue;
        Wall* w = it->second;

        fallingTimers[i] += gameTime;
        if (fallingTimers[i] >= fd.delay && !w->IsFalling())
            w->StartFalling(0.0f);

        if (w->IsFalling() && w->Y() >= 650.0f)
        {
            w->StopFalling();
            fallingDone[i] = true;
        }
    }

    // ── 5. Countdown de morte ──────────────────────────────────────
    if (parsedCountdown.active && parsedCountdown.seconds > 0.0f)
    {
        parsedCountdown.seconds -= gameTime;
        if (parsedCountdown.seconds < 0.0f) parsedCountdown.seconds = 0.0f;

        float urgency = 1.0f - (parsedCountdown.seconds /
                                 parsedCountdown.seconds); // normaliza dinâmico
        // Cor branca → vermelha conforme urgência
        float ratio = parsedCountdown.seconds <= 20.0f
                      ? parsedCountdown.seconds / 20.0f : 1.0f;
        Color timerColor(1.0f, ratio, ratio, 1.0f);

        std::string timerText = "TEMPO: "
            + std::to_string((int)ceilf(parsedCountdown.seconds)) + "s";
        DrawCentralMessage(timerText, timerColor, -1.0f, 155.0f);

        if (parsedCountdown.seconds <= 5.0f)
            DrawCentralMessage("!! CORRA !!",
                               Color(1.0f, 0.0f, 0.0f, 1.0f), -1.0f, 175.0f);

        if (parsedCountdown.seconds <= 0.0f)
        {
            parsedCountdown.active = false;
            player->SetHp(0);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// HUD
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::DrawCentralMessage(const std::string& text,
                                    Color color, float x, float y)
{
    if (text.empty() || consolas == nullptr) return;

    float finalX = x;
    if (x == -1.0f)
    {
        float textWidth = text.length() * 8.0f;
        finalX = window->CenterX() - (textWidth / 2.0f);
    }
    consolas->Draw(finalX, y, text, color);
}

void LevelMake::DrawHeartHealth()
{
    if (player == nullptr || heartSprite == nullptr) return;

    const int heartCount = 10;
    const int maxHp      = player->GetMaxHp();
    int       hp         = player->GetHp();
    if (hp < 0) hp = 0;

    int filledHearts = 0;
    if (maxHp > 0 && hp > 0)
        filledHearts = (hp * heartCount + maxHp - 1) / maxHp;
    if (filledHearts > heartCount) filledHearts = heartCount;

    const float scale      = 2.0f;
    const float spacing    = 6.0f;
    const float heartWidth = heartSprite->Width() * scale;
    const float totalWidth = (heartWidth * heartCount)
                           + (spacing * (heartCount - 1));
    const float startX     = window->CenterX() - (totalWidth / 2.0f)
                           + (heartWidth / 2.0f);
    const float y          = 72.0f;

    for (int i = 0; i < heartCount; i++)
    {
        const bool  filled = i < filledHearts;
        const Color color  = filled
            ? Color(1.0f, 1.0f, 1.0f, 1.0f)
            : Color(0.2f, 0.2f, 0.2f, 0.35f);
        heartSprite->Draw(startX + i * (heartWidth + spacing),
                          y, Layer::FRONT, scale, 0.0f, color);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::Init(float gravity, int maxFood, int maxGhost, string levelBackground)
{
    transitionBlock = new Sprite("Resources/Tijolo.png");
    isOpening = true;
    openingTimer = 0.0f;
    startDelay = 5;
    // 1. Zera todos os ponteiros de fase para NÃO ler lixo de memória!
    stages = nullptr;
    activePortals = nullptr;
    bgCount = 0;
    currentBG = 0;

    // 2. Inicializa as regras e a física
    Physics::Setup(gravity);
    MAX_GHOSTS = maxGhost;
    ghostAlive = MAX_GHOSTS;

    scene = new Scene();

    // 3. Inicializa as artes do HUD
    backg = nullptr;
    if (!levelBackground.empty())
        backg = new Sprite(levelBackground);

    heartSprite = new Sprite("Resources/Heart.png");

    // Garante que a fonte dos textos exista (evita crash no DrawCentralMessage)
    consolas = new Font("Resources/consolas12.png");
    consolas->Spacing("Resources/consolas12.dat");

    // 4. Cria o jogador
    player = new Player();
    scene->Add(player, MOVING);
}

// ─────────────────────────────────────────────────────────────────────────────
// Finalize
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::Finalize()
{
    // Walls e KillZones do parser foram adicionadas via scene->Add(),
    // a Scene é dona — apenas limpamos os maps locais.
    parsedWalls.clear();
    parsedKillZones.clear();
    parsedSpikes.clear();
    parsedPlatforms.clear();
    parsedSpinners.clear();
    parsedFalling.clear();
    parsedHazards.clear();
    spikeTimers.clear();
    spikeAliveTimers.clear();
    spikeActive.clear();
    platformPhases.clear();
    fallingTimers.clear();
    fallingDone.clear();

    if (stages != nullptr)
    {
        for (int i = 0; i < bgCount; i++)
        {
            delete stages[i].background;
            stages[i].background = nullptr;
            delete[] stages[i].portals;
            stages[i].portals = nullptr;
        }
        delete[] stages;
        stages = nullptr;
    }

    delete[] activePortals;
    activePortals     = nullptr;
    activePortalCount = 0;

    delete scene;
    scene = nullptr;

    delete backg;
    backg = nullptr;

    delete heartSprite;
    heartSprite = nullptr;

    delete consolas;
    consolas = nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Update
// ─────────────────────────────────────────────────────────────────────────────
void LevelMake::Update()
{
    if (isOpening) {
        if (startDelay > 0) {
            startDelay--;
        }
        else {
            float tempoSeguro = (gameTime > 0.1f) ? 0.016f : gameTime;
            openingTimer += tempoSeguro;

            if (openingTimer >= 0.5f) {
                isOpening = false; 
            }
        }
    }
    scene->Update();
    scene->CollisionDetection();
    UpdateStageTransition(gameTime);
    UpdateParsedMechanics();
    totalPlayTime += gameTime;

    if (stages != nullptr && !stages[currentBG].visited)
    {
        if (ghostAlive == 0 && !foodSpawned)
        {
            addFood(currentBG);
            foodSpawned = true;
        }

        if (comeuItem)
        {
            stages[currentBG].visited = true;
            CreatePortalsForCurrentStage();
            foodSpawned = false;
            comeuItem = false;
        }
    }

    if (window->KeyPress('B')) viewBBox = !viewBBox;

    if (player->GetHp() <= 0 || (player->totalLevelsVisited == 9))
    {
        std::string filename = "player_stats.txt";
        std::ofstream file(filename);
        std::string finalState = (player->GetHp() > 0) ? "VIVO" : "MORTO";

        if (file.is_open())
        {
            file << "--- STATUS DO JOGADOR ---" << std::endl;
            file << "Total de dano sofrido: " << totalDamageTaken / 10 << std::endl;
            file << "Total de dano deferido: " << player->totalDamageDealt / 10 << std::endl;
            file << "Total de inimigos derrotados: " << totalEnemiesDefeated << std::endl;
            file << "Tempo total de jogo: " << (int)totalPlayTime << "s" << std::endl;
            file << "Estado final: " << finalState << std::endl;
            file.close();
        }
        Engine::Next<Home>();
    }
}


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
        if (ghostAlive > 0)
            DrawCentralMessage("DERROTE OS INIMIGOS",
                Color(1.0f, 0.5f, 0.0f, 1.0f), -1.0f, 40.0f);
        else if (!comeuItem)
            DrawCentralMessage("ITEM DISPONIVEL! CONSUMA PARA SAIR",
                Color(0.4f, 0.8f, 1.0f, 1.0f), -1.0f, 40.0f);
        else
            DrawCentralMessage("PORTAS ABERTAS!",
                Color(0.2f, 1.0f, 0.2f, 1.0f), -1.0f, 40.0f);

        DrawHeartHealth();

        std::string stageStr = "ESTAGIO: " + std::to_string(currentBG + 1);
        DrawCentralMessage(stageStr, Color(0.8f, 0.8f, 1.0f, 1.0f), -1.0f, 112.0f);
    }
    if (isOpening && transitionBlock) {
        float progress = openingTimer / 0.5f;
        if (progress > 1.0f) progress = 1.0f;

        Color preto(0.0f, 0.0f, 0.0f, 1.0f);
        float size = 32.0f;

        int colunas = (window->Width() / size) + 1;
        int linhas = (window->Height() / size) + 1;

        for (int c = 0; c < colunas; c++) {
            for (int r = 0; r < linhas; r++) {

                int numeroSorteado = ((c * 89) ^ (r * 43)) % 100;

                // MÁGICA INVERSA: O número sorteado é invertido para dar o efeito de "rebobinar"
                float momentoParaSumir = 1.0f - (numeroSorteado / 100.0f);

                // Só desenha o bloco se o tempo atual AINDA NÃO passou do momento dele sumir
                if (progress < momentoParaSumir) {
                    float x = (c * size) + (size / 2.0f);
                    float y = (r * size) + (size / 2.0f);
                    transitionBlock->Draw(x, y, Layer::FRONT, 2.0f, 0.0f, preto);
                }
            }
        }
    }

    if (viewBBox) scene->DrawBBox();
}


// ─────────────────────────────────────────────────────────────────────────────
// ghostInit / foodInit
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::ghostInit(int stageIndex)
{
    for (int i = 0; i < MAX_GHOSTS; i++)
        addSnowman(stageIndex);
}




Enemy* LevelMake::addSnowman(int stageIndex) {
    Enemy* snowman = new Enemy();
    snowman->SetTarget(player);

    float margin = 50.0f;
    int rangeX = (int)((PlayArea::Right  - PlayArea::Left)   - (margin * 2));
    int rangeY = (int)((PlayArea::Bottom - PlayArea::Top)    - (margin * 2));
    if (rangeX <= 0) rangeX = 1;
    if (rangeY <= 0) rangeY = 1;

    float randomX, randomY, dist;
    do {
        randomX = (float)(rand() % rangeX) + PlayArea::Left + margin;
        randomY = (float)(rand() % rangeY) + PlayArea::Top  + margin;
        float dx = randomX - player->X();
        float dy = randomY - player->Y();
        dist = sqrtf(dx * dx + dy * dy);
    } while (dist < 400.0f);

    snowman->MoveTo(randomX, randomY);

    if (stageIndex == currentBG) {
        scene->Add(snowman, MOVING);
    }
    return snowman;
}



Food* LevelMake::addFood(int stageIndex) {
    Food* food = new Food();
    if (stageIndex == currentBG)
        scene->Add(food, STATIC);
    return food;
}

// ─────────────────────────────────────────────────────────────────────────────
// SetStage / ChangeBackground
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::SetStage(int index)
{
    if (index < 0 || index >= bgCount || stages == nullptr) return;

    ClearActivePortals();
    currentBG = index;

    if (player != nullptr && player->totalLevelsVisited == 0)
        player->MoveTo(stages[currentBG].spawnX, stages[currentBG].spawnY);

    if (!stages[currentBG].visited)
    {
        ghostInit(currentBG);
        ghostAlive = MAX_GHOSTS;
        CreateClosedDoorsForCurrentStage();
    }
    else
    {
        CreatePortalsForCurrentStage();
    }
}

void LevelMake::ChangeBackground(int index)
{
    if (index >= 0 && index < bgCount) currentBG = index;
}

// ─────────────────────────────────────────────────────────────────────────────
// Spawn helpers
// ─────────────────────────────────────────────────────────────────────────────

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

// ─────────────────────────────────────────────────────────────────────────────
// Portais
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::UpdateStageTransition(float dt)
{
    if (changingStage)
    {
        changeCooldown -= dt;
        if (changeCooldown <= 0.0f) changingStage = false;
    }
}

void LevelMake::CreatePortalsForCurrentStage()
{
    ClearActivePortals();
    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0)
    {
        activePortals = new Entity*[activePortalCount];
        for (int i = 0; i < activePortalCount; i++)
        {
            PortalData& data = stages[currentBG].portals[i];
            Portal* p = new Portal(PortalX(data.x), PortalY(data.y),
                                   data.targetBG, true,
                                   PortalRotation(data.x, data.y));
            activePortals[i] = p;
            scene->Add(p, STATIC);
        }
    }
}

void LevelMake::CreateClosedDoorsForCurrentStage()
{
    ClearActivePortals();
    activePortalCount = stages[currentBG].portalCount;
    if (activePortalCount > 0)
    {
        activePortals = new Entity*[activePortalCount];
        for (int i = 0; i < activePortalCount; i++)
        {
            PortalData& data = stages[currentBG].portals[i];
            Portal* p = new Portal(PortalX(data.x), PortalY(data.y),
                                   data.targetBG, false,
                                   PortalRotation(data.x, data.y));
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
    activePortals     = nullptr;
    activePortalCount = 0;
}

float LevelMake::PortalRotation(float x, float y) const
{
    const float pi = 3.14159265f;
    if (y < 100.0f)                      return 0.0f;
    if (y > window->Height() - 100.0f)  return pi;
    if (x < 100.0f)                      return -pi / 2.0f;
    if (x > window->Width()  - 100.0f)  return  pi / 2.0f;
    return 0.0f;
}

float LevelMake::PortalX(float x) const
{
    if (x < 100.0f)                     return 150;
    if (x > window->Width() - 100.0f)  return 1150;
    return x;
}

float LevelMake::PortalY(float y) const
{
    if (y < 100.0f)                      return 100;
    if (y > window->Height() - 100.0f)  return 690;
    return y;
}
