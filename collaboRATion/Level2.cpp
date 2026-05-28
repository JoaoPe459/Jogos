#include "Engine.h"
#include "Home.h"
#include "Level2.h"
#include <string>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

void Level2::Init()
{
    LevelMake::Init(600, 0, 0, "Resources/Level2.jpg");


    // ── 1. ESPINHOS DO CHÃO ──────────────────────────────────────────────────
    // Criados dinamicamente no Update; a Wall-âncora fica no centro inferior.
    spikeWall = new Wall();
    spikeWall->MoveTo(400.0f, 580.0f);
    scene->Add(spikeWall, STATIC);

    // ── 2. PAREDE QUE CAI DO TETO ────────────────────────────────────────────
    // Começa acima da área visível e cai após `fallDelay` segundos.
    ceilingWall = new Wall();
    ceilingWall->MoveTo(400.0f, -40.0f);   // fora da tela, acima
    ceilingWall->AddBlock(-66, 0, 32, 32, "Resources/Tijolo.png", "ceil_l");
    ceilingWall->AddBlock(0, 0, 32, 32, "Resources/Tijolo.png", "ceil_m");
    ceilingWall->AddBlock(66, 0, 32, 32, "Resources/Tijolo.png", "ceil_r");
    scene->Add(ceilingWall, STATIC);

    // ── 3. PLATAFORMA PING-PONG ──────────────────────────────────────────────
    movingWall = new Wall();
    movingWall->MoveTo(platformBaseX, platformBaseY);
    movingWall->AddBlock(-44, 0, 34, 32, "Resources/Tijolo.png", "plat_l");
    movingWall->AddBlock(0, 0, 34, 32, "Resources/Tijolo.png", "plat_m");
    movingWall->AddBlock(44, 0, 34, 32, "Resources/Tijolo.png", "plat_r");
    scene->Add(movingWall, STATIC);

    // ── 4. PAREDE ROTACIONANTE ────────────────────────────────────────────────
    // Dois blocos opostos formam um "moinho" giratório no centro do cenário.
    spinWall = new Wall();
    spinWall->MoveTo(640.0f, 350.0f);
    spinWall->AddBlock(-80, 0, 32, 32, "Resources/Tijolo.png", "spin_l");
    spinWall->AddBlock(80, 0, 32, 32, "Resources/Tijolo.png", "spin_r");
    scene->Add(spinWall, STATIC);

    // ── 5. ZONA DE DANO ───────────────────────────────────────────────────────
    // Bloco largo no chão que drena HP enquanto o player estiver em cima.
    hazardZone = new Wall();
    hazardZone->MoveTo(300.0f, 660.0f);
    hazardZone->AddBlock(-88, 0, 88, 22, "Resources/Tijolo.png", "haz_l");
    hazardZone->AddBlock(0, 0, 88, 22, "Resources/Tijolo.png", "haz_m");
    hazardZone->AddBlock(88, 0, 88, 22, "Resources/Tijolo.png", "haz_r");
    scene->Add(hazardZone, STATIC);
    
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers — cada mecânica isolada em seu próprio método
// ─────────────────────────────────────────────────────────────────────────────

// ── 1. Espinhos ───────────────────────────────────────────────────────────────
// Lógica: timer conta até spikeInterval → adiciona um bloco de espinho
//         enquanto ativo, conta spikeAlive → remove ao expirar OU se o player
//         chegar perto (RemoveBlocksInRadius).
void Level2::UpdateSpikes()
{
    if (!spikeActive)
    {
        spikeTimer += gameTime;
        if (spikeTimer >= spikeInterval)
        {
            spikeTimer = 0.0f;
            spikeActive = true;
            spikeAlive = 0.0f;

            // Escolhe uma posição X aleatória ao longo do chão
            float randX = (float)(rand() % 700) + 50.0f;
            spikeWall->MoveTo(randX, 580.0f);
            spikeWall->AddBlock(0, 0, 36, 36, "Resources/Tijolo.png", "spike");
        }
    }
    else
    {
        spikeAlive += gameTime;

        // Remove por proximidade do player
        spikeWall->RemoveBlocksInRadius(player->X(), player->Y(), 50.0f);

        // Remove por expiração de tempo
        if (spikeAlive >= spikeDuration)
        {
            spikeWall->RemoveBlock("spike");
            spikeActive = false;
        }
    }
}

// ── 2. Parede que cai ─────────────────────────────────────────────────────────
// Aguarda fallDelay segundos e então chama StartFalling().
// Para quando atingir o chão (Y > 700).
void Level2::UpdateCeilingWall()
{
    if (hasFallen) return;

    fallTimer += gameTime;
    if (fallTimer >= fallDelay && !ceilingWall->IsFalling())
    {
        ceilingWall->StartFalling(0.0f);
    }

    // Para ao atingir o chão
    if (ceilingWall->IsFalling() && ceilingWall->Y() >= 650.0f)
    {
        ceilingWall->StopFalling();
        hasFallen = true;
    }
}

// ── 3. Plataforma ping-pong ───────────────────────────────────────────────────
// Usa seno para oscilar suavemente. SyncToOwner() mantém os blocos alinhados.
void Level2::UpdateMovingPlatform()
{
    platformPhase += platformSpeed * gameTime;

    float newX = platformBaseX + sinf(platformPhase) * platformRange;
    movingWall->MoveTo(newX, platformBaseY);

    for (Block* b : movingWall->GetBlocks())
        b->SyncToOwner();
}

// ── 4. Parede rotacionante ────────────────────────────────────────────────────
// Gira continuamente. Rotate() já chama SyncToOwner internamente.
void Level2::UpdateSpinWall()
{
    spinWall->Rotate(spinSpeed * gameTime);
}

// ── 5. Countdown de morte ─────────────────────────────────────────────────────
// Exibe o tempo restante em vermelho no HUD. Mata o player ao chegar em zero.
void Level2::UpdateCountdown()
{
    if (!countdownActive) return;

    countdown -= gameTime;
    if (countdown < 0.0f) countdown = 0.0f;

    // Cor vai do branco para vermelho conforme o tempo diminui
    float urgency = 1.0f - (countdown / 20.0f);   // 0..1
    Color timerColor(1.0f, 1.0f - urgency, 1.0f - urgency, 1.0f);

    std::string timerText = "TEMPO: " + std::to_string((int)ceil(countdown)) + "s";
    DrawCentralMessage(timerText, timerColor, -1.0f, 155.0f);

    if (countdown <= 5.0f)
        DrawCentralMessage("!! CORRA !!", Color(1.0f, 0.0f, 0.0f, 1.0f), -1.0f, 175.0f);

    if (countdown <= 0.0f)
    {
        countdownActive = false;
        player->SetHp(0);   // mata o player
    }
}

// ── 6. Zona de dano contínuo ──────────────────────────────────────────────────
// Drena HP por tick se o player estiver dentro do raio da zona.
void Level2::UpdateHazardZone()
{
    float dx = player->X() - hazardZone->X();
    float dy = player->Y() - hazardZone->Y();
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist <= 100.0f)   // raio da zona de dano
    {
        hazardTickTimer += gameTime;
        if (hazardTickTimer >= hazardTickInterval)
        {
            hazardTickTimer = 0.0f;
            int newHp = player->GetHp() - hazardDamage;
            player->SetHp(newHp < 0 ? 0 : newHp);
        }
        DrawCentralMessage("ZONA DE PERIGO!", Color(1.0f, 0.3f, 0.0f, 1.0f), -1.0f, 195.0f);
    }
    else
    {
        hazardTickTimer = 0.0f;   // reseta o tick se o player sair da zona
    }
}

// ── HUD extra ─────────────────────────────────────────────────────────────────
void Level2::DrawHUD()
{
    // Legenda de controles (canto inferior esquerdo)
    if (consolas == nullptr) return;
    DrawCentralMessage("[WASD] Mover  [Setas] Atirar  [ESC] Sair", Color(0.6f, 0.6f, 0.6f, 0.8f), 20.0f, 730.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// Update principal
// ─────────────────────────────────────────────────────────────────────────────

void Level2::Update()
{
    if (window->KeyPress(VK_ESCAPE))
    {
        Engine::Next<Home>();
        return;
    }

    // ── Mecânicas Level Devil ────────────────────────────────────────────────
    UpdateSpikes();
    UpdateCeilingWall();
    UpdateMovingPlatform();
    UpdateSpinWall();
    UpdateCountdown();
    UpdateHazardZone();

    // ── Controles de debug (manter os que já existiam) ───────────────────────
    if (window->KeyDown('R'))  spinWall->Rotate(+180.0f * gameTime);
    if (window->KeyDown('E'))  spinWall->Rotate(-180.0f * gameTime);
    if (window->KeyPress('F')) ceilingWall->StartFalling();
    if (window->KeyPress('G')) ceilingWall->StopFalling();

    if (window->KeyPress('1')) spinWall->SetAngle(0.0f);
    if (window->KeyPress('2')) spinWall->SetAngle(45.0f);
    if (window->KeyPress('3')) spinWall->SetAngle(90.0f);

    // ── Base do LevelMake (scene update, colisões, HUD de vida etc.) ─────────
    DrawHUD();
    LevelMake::Update();
}

// ─────────────────────────────────────────────────────────────────────────────
// Finalize
// ─────────────────────────────────────────────────────────────────────────────

void Level2::Finalize()
{
    // As Walls foram adicionadas à Scene via scene->Add(), então a Scene
    // é dona da memória delas. Apenas zeramos os ponteiros locais.
    spikeWall = nullptr;
    ceilingWall = nullptr;
    movingWall = nullptr;
    spinWall = nullptr;
    hazardZone = nullptr;

    LevelMake::Finalize();
}