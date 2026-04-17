/**********************************************************************************
// LevelMake (Arquivo de Cabeçalho)
//
// Criação:     18 Jan 2013
// Atualização: 17 Apr 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Sistema de estágios com HUB e portais circulares
//
**********************************************************************************/

#ifndef _PACMAN_LEVELMAKE_H_
#define _PACMAN_LEVELMAKE_H_

// ------------------------------------------------------------------------------
// Inclusões

#include "Game.h"
#include "Scene.h"
#include "Entity.h"
#include <vector>
#include <string>
#include "Physics.h"
#include "Player.h"
#include "Engine.h"
#include "Ghost.h"
#include "Food.h"
#include "Wall.h"

// ------------------------------------------------------------------------------
// Estruturas

struct PortalData {
    float x, y;
    int targetBG;

    // Opcional: direção do portal (útil pra spawn offset)
    int direction; // 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
};

// Definição de um cenário completo
struct StageConfig {
    Sprite* background;
    PortalData* portals;
    int portalCount;
    float spawnX, spawnY;
};

// ------------------------------------------------------------------------------

class Home;

class LevelMake : public Game
{
protected:
    Sprite* backg = nullptr;
    Scene* scene = nullptr;
    Sprite* foodSprite = nullptr;

    Font* consolas = nullptr;

    bool viewBBox = false;

    StageConfig* stages = nullptr;
    int bgCount = 0;
    int currentBG = 0;

    Entity** activePortals = nullptr;
    int activePortalCount = 0;

    int MAX_FOOD = 0;
    int MAX_GHOSTS = 0;

    Player* player = nullptr;
    Food** foods = nullptr;
    Entity** entities = nullptr;

    int foodCount = 0;
    int entityCount = 0;

    // -------- CONTROLE DE TRANSIÇÃO --------
    bool changingStage = false;
    float changeCooldown = 0.0f;

    void LoadLevel(std::string path);

public:
    // -------- CICLO DO JOGO --------
    void Init() override;
    void Init(float, int, int, std::string);
    void Update() override;
    void Draw() override;
    void Finalize() override;

    // -------- ACESSO À CENA --------
    Scene* GetScene() { return scene; }

    // -------- INICIALIZAÇÃO --------
    void ghostInit();
    void foodInit();
    void GenerateMaze(Scene* scene, Window* window, int tileSize);

    // -------- SISTEMA DE STAGE --------
    void ChangeBackground(int index);
    void SetStage(int index);

    float GetSpawnX(int index) const;
    float GetSpawnY(int index) const;

    // -------- CONTROLE DE PORTAL (NOVO) --------
    bool IsChangingStage() const { return changingStage; }

    void BeginStageChange() { changingStage = true; }

    void SetStageChangeCooldown(float time) {
        changeCooldown = time;
    }

    void UpdateStageTransition(float dt);

    int GetCurrentStage() const { return currentBG; }
};

#endif