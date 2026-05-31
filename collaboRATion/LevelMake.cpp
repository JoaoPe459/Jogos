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
#include "Level2.h"
#include "Interactables.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>

using std::ifstream;
using std::string;
bool LevelMake::modInvertControls = false;
float LevelMake::modJumpForce = 1.0f;
bool LevelMake::modGravityJump = false;
bool LevelMake::modMoveWorld = false;
int LevelMake::deathCount = 0;
std::vector<std::string> LevelMake::mapasDoNivel = {};
int LevelMake::faseAtual = 0;
bool LevelMake::avancarFase = false;
Audio* LevelMake::audioEngine = nullptr;
int  LevelMake::nivelDesbloqueado = 0;


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
        if (key == "GRAVITY") {
            float g;
            ss >> g;
            Physics::Setup(g);
        }
        // ── Wall base ────────────────────────────────────────────
        else if (key == "WALL") {
            std::string id;
            float wx, wy;
            ss >> id >> wx >> wy;

            Wall* w = new Wall();
            w->MoveTo(wx, wy);
            scene->Add(w, STATIC);
            parsedWalls[id] = w;
            parsedObjects[id] = w; // <--- FALTAVA ISTO!
        }
        else if (key == "BOLA") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Wall* w = new Wall(); w->MoveTo(wx, wy);
            w->AddBlock(0, 0, 32, 32, "Resources/Bola.png", id);
            scene->Add(w, STATIC);
            parsedWalls[id] = w;
            parsedObjects[id] = w; // <--- FALTAVA ISTO!
        }
        else if (key == "ESPINHO") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Spike* spikeCima = new Spike(wx, wy, "Resources/Espinhos.png", false);
            scene->Add(spikeCima, STATIC);
            parsedObjects[id] = spikeCima; // <--- FALTAVA ISTO!
        }
        else if (key == "ESPINHO_INV") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Spike* spikeBaixo = new Spike(wx, wy, "Resources/Espinhos2.png", true);
            scene->Add(spikeBaixo, STATIC);
            parsedObjects[id] = spikeBaixo; // <--- FALTAVA ISTO!
        }
        else if (key == "PORTA") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            Door* door = new Door(wx, wy, "Resources/Porta.png");
            scene->Add(door, STATIC);
            parsedObjects[id] = door; // <--- FALTAVA ISTO!
        }
        else if (key == "BACKGROUND") {
            std::string spritePath;
            ss >> spritePath;
            Sprite* novo = new Sprite(spritePath);
            if (novo != nullptr) {
                delete backg;
                backg = novo;
            }
        }
        else if (key == "SPAWN") {
            float x, y;
            ss >> x >> y;
            if (player) player->MoveTo(x, y);
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
        else if (key == "BOTAO") {
            std::string id; float wx, wy;
            ss >> id >> wx >> wy;
            ButtonObj* btn = new ButtonObj(wx, wy, id);
            scene->Add(btn, STATIC);
            parsedButtons[id] = btn;
            parsedObjects[id] = btn; // <--- FALTAVA ISTO!
        }
        else if (key == "MOD_TOGGLE") {
            std::string btnId, modType;
            ss >> btnId >> modType;

            modToggles[btnId] = modType;
            modToggleState[btnId] = false; // Começa sem estar pisado
        }

        else if (key == "PATH")
        {
            PathData pd;
            pd.dir = 1;
            int loopInt, count;

            ss >> pd.wallId >> pd.speed >> pd.triggerMode >> pd.triggerParam >> loopInt >> count;

            pd.isLoop = (loopInt != 0);

            for (int i = 0; i < count; i++) {
                Waypoint wp;
                ss >> wp.x >> wp.y;
                pd.pts.push_back(wp);
            }
            parsedPaths.push_back(pd);
        }
        else if (key == "TRIGGER") {
            TriggerZone tz; std::string id;
            ss >> id >> tz.x >> tz.y >> tz.w >> tz.h;

            parsedTriggers[id] = tz;
        }
        else if (key == "MOD_INVERT_CONTROLS") {
            int val; ss >> val;
            modInvertControls = (val != 0); // Ex: MOD_INVERT_CONTROLS 1
        }
        else if (key == "MOD_JUMP_FORCE") {
            ss >> modJumpForce;             // Ex: MOD_JUMP_FORCE 2.5
        }
        else if (key == "MOD_GRAVITY_JUMP") {
            int val; ss >> val;
            modGravityJump = (val != 0);    // Ex: MOD_GRAVITY_JUMP 1
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
        else if (key == "KILLZONE") {
            std::string id, tag;
            float kx, ky, kw, kh;
            int   lethalInt;
            ss >> id >> kx >> ky >> kw >> kh >> lethalInt;
            ss >> tag;

            KillZone* kz = new KillZone(kx, ky, kw, kh, lethalInt != 0, tag);
            scene->Add(kz, STATIC);
            parsedKillZones[id] = kz;
            parsedObjects[id] = kz; 
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
    // ── 6. Sistema de Waypoints e Proximidade ─────────────────────
    for (auto& pd : parsedPaths)
    {
        Object* objMovel = nullptr;
        TriggerZone* trigMovel = nullptr;

        // 1. Procura o objeto na LISTA DE FÍSICA (Paredes, Portas, Espinhos...)
        auto itObj = parsedObjects.find(pd.wallId);
        if (itObj != parsedObjects.end()) {
            objMovel = itObj->second;
        }
        else {
            // 2. Se não achou na física, procura na LISTA DE GATILHOS INVISÍVEIS!
            auto itTrig = parsedTriggers.find(pd.wallId);
            if (itTrig != parsedTriggers.end()) {
                trigMovel = &itTrig->second;
            }
        }

        // Se não achou em nenhuma das duas listas, salta para o próximo caminho
        if (!objMovel && !trigMovel) continue;

        // Guarda a posição atual (seja de um objeto físico ou de um gatilho)
        float curX = objMovel ? objMovel->X() : trigMovel->x;
        float curY = objMovel ? objMovel->Y() : trigMovel->y;

        // --- VERIFICA OS GATILHOS PARA ACORDAR A PLATAFORMA/ESPINHO/GATILHO ---
        if (!pd.active) {
            if (pd.triggerMode == 0) {
                // MODO 0: DISTÂNCIA
                float distToTrigger = std::stof(pd.triggerParam);
                if (distToTrigger <= 0.0f) {
                    pd.active = true;
                }
                else {
                    float dx = player->X() - curX;
                    float dy = player->Y() - curY;
                    if (sqrtf(dx * dx + dy * dy) <= distToTrigger) pd.active = true;
                }
            }
            else if (pd.triggerMode == 1) {
                    if (objMovel) {
                        if (scene->Collision(player, objMovel) && player->Y() < objMovel->Y()) {
                            pd.active = true;
                        }
                    else if (trigMovel) {
                        // Calcula as 4 paredes invisíveis do Gatilho
                        float left = trigMovel->x - 16.0f;
                        float right = left + trigMovel->w;
                        float top = trigMovel->y - 16.0f;
                        float bottom = top + trigMovel->h;

                        // Se o Rato estiver dentro desta "caixa", ativa!
                        if (player->X() >= left && player->X() <= right &&
                            player->Y() >= top && player->Y() <= bottom) {
                            pd.active = true;
                        }
                    }
                }
            }
            else if (pd.triggerMode == 2) {
                auto itTrig = parsedTriggers.find(pd.triggerParam);
                if (itTrig != parsedTriggers.end()) {

                    float left = itTrig->second.x - 16.0f;
                    float right = left + itTrig->second.w;
                    float top = itTrig->second.y - 16.0f;
                    float bottom = top + itTrig->second.h;

                    if (player->X() >= left && player->X() <= right &&
                        player->Y() >= top && player->Y() <= bottom) {
                        pd.active = true;
                    }
                }
                else {
                    auto itBtn = parsedButtons.find(pd.triggerParam);
                    if (itBtn != parsedButtons.end()) {
                        if (itBtn->second->IsPressed()) pd.active = true;
                    }
                }
            }
        }

        // --- MOVIMENTA O OBJETO UNIVERSAL ---
        if (pd.active && pd.pts.size() > 1) {
            Waypoint target = pd.pts[pd.currentPt];
            float dx = target.x - curX;
            float dy = target.y - curY;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist < pd.speed * gameTime) {
                // Crava o objeto exatamente no ponto
                if (objMovel) objMovel->MoveTo(target.x, target.y);
                if (trigMovel) { trigMovel->x = target.x; trigMovel->y = target.y; }

                if (!pd.isLoop && pd.currentPt == (int)pd.pts.size() - 1) {
                    // Chegou ao fim, desliga!
                    pd.active = false;
                }
                else {
                    pd.currentPt += 1;
                    if (pd.currentPt >= (int)pd.pts.size()) {
                        if (pd.isLoop) pd.currentPt = 0;
                    }
                }
            }
            else {
                // Continua a andar em direção ao alvo
                float newX = curX + (dx / dist) * pd.speed * gameTime;
                float newY = curY + (dy / dist) * pd.speed * gameTime;

                if (objMovel) objMovel->MoveTo(newX, newY);
                if (trigMovel) { trigMovel->x = newX; trigMovel->y = newY; }
            }

            // Sincroniza os blocos secundários APENAS se o objeto for uma Parede (Wall)
            if (objMovel) {
                auto itWall = parsedWalls.find(pd.wallId);
                if (itWall != parsedWalls.end()) {
                    for (Block* b : itWall->second->GetBlocks()) b->SyncToOwner();
                }
            }
        }
    }
    for (auto& par : modToggles) {
        std::string id = par.first;
        std::string modType = par.second;

        bool isAtivo = false;

        auto itTrig = parsedTriggers.find(id);
        if (itTrig != parsedTriggers.end()) {

            float left = itTrig->second.x - 16.0f;
            float right = left + itTrig->second.w;
            float top = itTrig->second.y - 16.0f;
            float bottom = top + itTrig->second.h;

            // Verifica se o Rato está dentro da caixa exata
            if (player->X() >= left && player->X() <= right &&
                player->Y() >= top && player->Y() <= bottom) {
                isAtivo = true;
            }
        }
        else {
            // 2. SEGUNDO: Se não for Gatilho, tenta ver se o ID é de um BOTÃO
            auto itBtn = parsedButtons.find(id);
            if (itBtn != parsedButtons.end()) {
                isAtivo = itBtn->second->IsPressed();
            }
        }

        // 3. A MÁGICA: Só inverte a regra no exato frame em que entrou na área ou pisou no botão!
        if (isAtivo && !modToggleState[id]) {
            modToggleState[id] = true; // Marca que já ativou

            // Alterna as regras do universo (True vira False, False vira True)
            if (modType == "INVERT_CONTROLS") modInvertControls = !modInvertControls;
            else if (modType == "GRAVITY_JUMP") modGravityJump = !modGravityJump;
        }
        // Quando o rato sai de cima do botão ou sai da área do gatilho, reseta para poder ser usado de novo
        else if (!isAtivo && modToggleState[id]) {
            modToggleState[id] = false;
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



// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

void LevelMake::Init(float gravity, int maxFood, int maxGhost, string levelBackground)
{
    transitionBlock = new Sprite("Resources/Tijolo.png");
    isOpening = true;
    openingTimer = 0.0f;
    startDelay = 5;
    avancarFase = false;

    modInvertControls = false;
    modJumpForce = 1.0f;
    modGravityJump = false;
    modMoveWorld = false;
    if (audioEngine == nullptr) {
        audioEngine = new Audio();
        audioEngine->Add(JUMP_ID, "Resources/Sounds/jump.wav");
        audioEngine->Add(DEATH_ID, "Resources/Sounds/death.wav");
        audioEngine->Add(BUTTON_ID, "Resources/Sounds/botao.wav");
        audioEngine->Add(TOC_ID, "Resources/Sounds/toc.wav");
        audioEngine->Add(GRAVITY_ID, "Resources/Sounds/invertergravidade.wav");
        audioEngine->Add(WALK_ID, "Resources/Sounds/passos.wav");
        audioEngine->Add(GAME_SOUND_ID, "Resources/Sounds/gamesound.wav");

        audioEngine->Play(GAME_SOUND_ID, true);
    }
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
    if (!levelBackground.empty())
        backg = new Sprite(levelBackground);
    else
        backg = new Sprite("Resources/Fundo.png");

    heartSprite = new Sprite("Resources/Heart.png");

    // Garante que a fonte dos textos exista (evita crash no DrawCentralMessage)
    consolas = new Font("Resources/consolas12.png");
    consolas->Spacing("Resources/consolas12.dat");
    terminal = new Font("Resources/terminal.png");
    terminal->Spacing(30);

    // 4. Cria o jogador
    player = new Player();
    scene->Add(player, MOVING);

    CarregarProgresso();
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
    modToggles.clear();
    modToggleState.clear();

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
    UpdateParsedMechanics();
    totalPlayTime += gameTime;

    if (window->KeyPress('B')) viewBBox = !viewBBox;

    if (player->isDead && window->KeyPress('R')) {
        Engine::Next<Level2>();

    }

}


void LevelMake::Draw()
{
    if (backg != nullptr && backg->Width() > 0)
    {
        float scale = float (window->Width() / backg->Width());
        backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK, scale);
    }

    if (consolas != nullptr) {
        std::string textoMortes = std::to_string(deathCount);

        float posX = window->Width() - 30.0f;
        float posY = 40.0f;

        terminal->Draw(posX, posY, textoMortes, Color(0.0f, 0.0f, 0.0f, 1.0f), Layer::FRONT,1);
    }

    // 2. Desenha os Objetos da Cena
    scene->Draw();
    
    if (isOpening && transitionBlock) {
        float progress = openingTimer / 0.5f;
        if (progress > 1.0f) progress = 1.0f;

        Color preto(0.0f, 0.0f, 0.0f, 1.0f);
        float size = 32.0f;

        int colunas = int ((window->Width() / size) + 1);
        int linhas = int ((window->Height() / size) + 1);

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
    if (player->isDead) {
        DrawCentralMessage("VOCE MORREU!", Color(1.0f, 0.0f, 0.0f, 1.0f), -1.0f, window->CenterY() - 20.0f);
        DrawCentralMessage("APERTE 'R' PARA REINICIAR", Color(1.0f, 1.0f, 1.0f, 1.0f), -1.0f, window->CenterY() + 10.0f);
    }

    if (viewBBox) scene->DrawBBox();
}


void LevelMake::SalvarProgresso()
{
    std::ofstream f("save.txt");
    if (f.is_open())
    {
        f << nivelDesbloqueado;
        f.close();
    }
}

void LevelMake::CarregarProgresso()
{
    std::ifstream f("save.txt");
    if (f.is_open())
    {
        f >> nivelDesbloqueado;
        f.close();
    }
}
