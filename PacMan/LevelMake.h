/**********************************************************************************
// LevelMake (Arquivo de Cabeçalho)
//
// Criação:     18 Jan 2013
// Atualização: 21 Apr 2026
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
#include "Enemy.h"

// ------------------------------------------------------------------------------
// Estruturas

struct PortalData {
    float x, y;
    int targetBG;
    int direction; // 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
};

// Definição de um cenário completo.
//
// ATENÇÃO — propriedade de memória:
//   background e portals   → StageConfig é dono, Finalize() os deleta.
//   ghosts[] e foods[]     → são arrays de RASTREAMENTO apenas.
//                            Os objetos pertencem à Scene (adicionados via Add()).
//                            Finalize() libera apenas os arrays, nunca os objetos.
struct StageConfig {
    Sprite* background = nullptr;
    PortalData* portals = nullptr;
    int         portalCount = 0;
    float       spawnX = 0.0f;
    float       spawnY = 0.0f;
	bool visited = false;

};

// ------------------------------------------------------------------------------

class Home;

class LevelMake : public Game
{
protected:
    Sprite* backg = nullptr;
    Scene* scene = nullptr;
    Sprite* foodSprite = nullptr;
    Sprite* heartSprite = nullptr;
    Font* consolas = nullptr;

    bool    viewBBox = false;

    StageConfig* stages = nullptr;
    int          bgCount = 0;
    int          currentBG = 0;

    // Rastreamento dos portais ativos na cena (Scene é dona dos objetos)
    Entity** activePortals = nullptr;
    int      activePortalCount = 0;

    int MAX_GHOSTS = 0;
    

    Player* player = nullptr;

    // -------- CONTROLE DE TRANSIÇÃO --------
    bool  changingStage = false;
    float changeCooldown = 0.0f;

    void LoadLevel(std::string path);
    

public:
    // -------- CICLO DO JOGO --------
    void Init() {};
    void Init(float, int, int, std::string);
    void Update()   override;
    void Draw()     override;
    void Finalize() override;

    // -------- ACESSO À CENA --------
    Scene* GetScene() { return scene; }

    // -------- INICIALIZAÇÃO DE ENTIDADES --------
    void ghostInit(int stageIndex);
    void foodInit(int stageIndex = -1);

    Ghost* addGhost(int stageIndex);

    Enemy* addSnowman(int stageIndex);

    Food* addFood(int stageIndex);

    // Sobrecargas de conveniência — operam sobre o estágio atual
    void ghostInit() { ghostInit(currentBG); }
    void foodInit() { foodInit(currentBG); }

    void Remove(Object* obj, int type);
    void GenerateMaze(Scene* scene, Window* window, int tileSize);

    // -------- SISTEMA DE STAGE --------
    void ChangeBackground(int index);
    void SetStage(int index);

    float GetSpawnX(int index) const;
    float GetSpawnY(int index) const;

    // -------- CONTROLE DE PORTAL --------
    bool IsChangingStage()  const { return changingStage; }
    void BeginStageChange() { changingStage = true; }
    void SetStageChangeCooldown(float time) { changeCooldown = time; }
    void UpdateStageTransition(float dt);
    int  GetCurrentStage() const { return currentBG; }
    void CreatePortalsForCurrentStage();
    void CreateClosedDoorsForCurrentStage();
    void ClearActivePortals();
    float PortalRotation(float x, float y) const;
    float PortalX(float x) const;
    float PortalY(float y) const;
    void DrawCentralMessage(const std::string& text, Color color, float x, float y);
    void DrawHeartHealth();
    
    int ghostAlive = 0;
    bool comeuItem = false;
    bool foodSpawned = false;
};

#endif
