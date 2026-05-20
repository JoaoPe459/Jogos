#ifndef _PACMAN_LEVEL2_H_
#define _PACMAN_LEVEL2_H_

#include "LevelMake.h"
#include "Wall.h"

// ─────────────────────────────────────────────────────────────────────────────
// MECÂNICAS ESTILO LEVEL DEVIL
//
//  1. Espinhos do chão    — blocos surgem em intervalos e somem por proximidade
//  2. Parede que cai      — Wall desce do teto com StartFalling() após delay
//  3. Plataforma ping-pong — Wall oscila horizontalmente com seno
//  4. Parede rotacionante — Wall.Rotate() contínuo no Update
//  5. Contador de morte   — timer visível que mata o player ao chegar em zero
//  6. Zona de dano        — bloco estático que drena HP por tick
// ─────────────────────────────────────────────────────────────────────────────

class Level2 : public LevelMake
{
public:
    void Init()     override;
    void Update()   override;
    void Finalize() override;

private:
    // ── 1. Espinhos ──────────────────────────────────────────────────────────
    Wall* spikeWall = nullptr;
    float  spikeTimer = 0.0f;
    float  spikeInterval = 2.5f;    // surge a cada X segundos
    float  spikeDuration = 1.2f;    // permanece X segundos antes de sumir
    float  spikeAlive = 0.0f;    // contador de quanto tempo o espinho está ativo
    bool   spikeActive = false;

    // ── 2. Parede que cai ────────────────────────────────────────────────────
    Wall* ceilingWall = nullptr;
    float  fallDelay = 4.0f;    // segundos até começar a cair
    float  fallTimer = 0.0f;
    bool   hasFallen = false;

    // ── 3. Plataforma ping-pong ──────────────────────────────────────────────
    Wall* movingWall = nullptr;
    float  platformPhase = 0.0f;
    float  platformSpeed = 1.8f;    // frequência da oscilação (rad/s)
    float  platformRange = 220.0f;  // amplitude do movimento (px)
    float  platformBaseX = 400.0f;
    float  platformBaseY = 450.0f;

    // ── 4. Parede rotacionante ────────────────────────────────────────────────
    Wall* spinWall = nullptr;
    float  spinSpeed = 90.0f;   // graus/segundo

    // ── 5. Contador de morte ──────────────────────────────────────────────────
    float  countdown = 20.0f;   // segundos restantes
    bool   countdownActive = true;

    // ── 6. Zona de dano contínuo ──────────────────────────────────────────────
    Wall* hazardZone = nullptr;
    float  hazardTickTimer = 0.0f;
    float  hazardTickInterval = 0.8f;  // dano a cada X segundos
    int    hazardDamage = 8;

    // ── Helpers ───────────────────────────────────────────────────────────────
    void UpdateSpikes();
    void UpdateCeilingWall();
    void UpdateMovingPlatform();
    void UpdateSpinWall();
    void UpdateCountdown();
    void UpdateHazardZone();
    void DrawHUD();
};

#endif