/**********************************************************************************
// LevelMake (Arquivo de Cabeçalho)
//
// Criação:     18 Jan 2013
// Atualização: 28 Mai 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Sistema de estágios com HUB e portais circulares.
//              Parser de níveis expandido com suporte a:
//                - KillZone (zona de morte/dano)
//                - Escala de Wall (SetScale)
//                - Gravidade variável por fase (GRAVITY)
//                - Mecânicas estilo Level Devil (SPIKE, PLATFORM, SPINNER,
//                  FALLING, HAZARD, COUNTDOWN)
//
**********************************************************************************/

#ifndef _PACMAN_LEVELMAKE_H_
#define _PACMAN_LEVELMAKE_H_

#include "Game.h"
#include "Scene.h"
#include "Entity.h"
#include <vector>
#include <string>
#include <map>
#include "Physics.h"
#include "Player.h"
#include "Engine.h"
#include "Ghost.h"
#include "Food.h"
#include "Wall.h"
#include "Enemy.h"
#include "EndGame.h"
#include "KillZone.h"
#include "Interactables.h"

// ─────────────────────────────────────────────────────────────────────────────
// Estruturas de configuração de portais
// ─────────────────────────────────────────────────────────────────────────────

struct PortalData {
    float x, y;
    int   targetBG;
    int   direction; // 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
};

struct StageConfig {
    Sprite*     background  = nullptr;
    PortalData* portals     = nullptr;
    int         portalCount = 0;
    float       spawnX      = 0.0f;
    float       spawnY      = 0.0f;
    bool        visited     = false;
};

// ─────────────────────────────────────────────────────────────────────────────
// Dados de mecânicas carregados pelo parser
// Cada struct corresponde a um token do .txt
// ─────────────────────────────────────────────────────────────────────────────

struct SpikeData {
    std::string wallId;
    float x, y;
    float interval;   // segundos entre aparições
    float duration;   // segundos que fica visível
    float randRange;  // largura da área aleatória em X
};

struct PlatformData {
    std::string wallId;
    float baseX, baseY;
    float speed;      // frequência (rad/s)
    float range;      // amplitude (px)
};

struct SpinnerData {
    std::string wallId;
    float speed;      // graus/segundo
};

struct FallingData {
    std::string wallId;
    float delay;      // segundos até começar a cair
};

struct HazardData {
    std::string kzId;
    float radius;     // raio de detecção (usa KillZone interna)
    int   damage;
    float interval;
};

struct CountdownData {
    float seconds;
    bool  active = true;
};
struct Waypoint { float x, y; };

struct PathData {
    std::string wallId;
    float speed;              // Velocidade controlável
    int triggerMode;          // 0=Distância, 1=Pisar, 2=Botão
    std::string triggerParam; // Valor da distância OU o ID do Botão
    bool isLoop;
    std::vector<Waypoint> pts;
    int currentPt = 1;
    int dir = 1;
    bool active = false;
};

struct TriggerZone { float x, y, w, h; };

// ─────────────────────────────────────────────────────────────────────────────

class Home;
class EndGame;

class LevelMake : public Game
{
protected:

    int startDelay = 0;
    Sprite* transitionBlock = nullptr;
    bool isOpening = true;
    float openingTimer = 0.0f;
    Sprite* backg       = nullptr;
    Scene*  scene       = nullptr;
    Sprite* foodSprite  = nullptr;
    Sprite* heartSprite = nullptr;
    Font*   consolas    = nullptr;

    bool viewBBox = false;

    StageConfig* stages   = nullptr;
    int          bgCount  = 0;
    

    Entity** activePortals     = nullptr;
    int      activePortalCount = 0;

    int MAX_GHOSTS = 0;

    Player* player = nullptr;

    // ── Controle de transição ────────────────────────────────────
    bool  changingStage  = false;
    float changeCooldown = 0.0f;

    // ── Walls/KillZones criadas pelo parser (indexadas por id) ───
    std::map<std::string, Wall*>      parsedWalls;
    std::map<std::string, KillZone*>  parsedKillZones;
    std::map<std::string, Object*>    parsedObjects;

    // ── Dados de mecânicas do parser ─────────────────────────────
    std::vector<SpikeData>    parsedSpikes;
    std::vector<PlatformData> parsedPlatforms;
    std::vector<SpinnerData>  parsedSpinners;
    std::vector<FallingData>  parsedFalling;
    std::vector<HazardData>   parsedHazards;
    CountdownData             parsedCountdown;

    // Timers de runtime para as mecânicas do parser
    // (indexados na mesma ordem dos vetores acima)
    std::vector<float> spikeTimers;
    std::vector<float> spikeAliveTimers;
    std::vector<bool>  spikeActive;
    std::vector<float> platformPhases;
    std::vector<float> fallingTimers;
    std::vector<bool>  fallingDone;
    std::vector<PathData> parsedPaths;

    std::map<std::string, ButtonObj*> parsedButtons;
    std::map<std::string, TriggerZone> parsedTriggers;

    // ── Leitura de nível ──────────────────────────────────────────
    void LoadLevel(std::string path);

    // ── Parser expandido (Level Devil) ───────────────────────────
    // Carrega um .txt com walls, killzones, mecânicas e gravidade.
    // Deve ser chamado após LevelMake::Init().
    void LoadLevel2(const std::string& path);

    // Atualiza todas as mecânicas carregadas pelo parser
    void UpdateParsedMechanics();

public:
    // ── Ciclo do jogo ─────────────────────────────────────────────
    void Init() {};
    void Init(float gravity, int maxFood, int maxGhost, std::string levelBackground);
    void Update()   override;
    void Draw()     override;
    void Finalize() override;

    // ── Acesso à cena ─────────────────────────────────────────────
    Scene* GetScene() { return scene; }

    // ── Gravidade ─────────────────────────────────────────────────
    void SetGravity(float g) { Physics::Setup(g); }

    // ── Inicialização de entidades ────────────────────────────────
    void   ghostInit(int stageIndex);
    Enemy* addSnowman(int stageIndex);
    Food*  addFood(int stageIndex);

    void ghostInit() { ghostInit(currentBG); }


    // ── Sistema de stage ──────────────────────────────────────────
    void  ChangeBackground(int index);
    void  SetStage(int index);
    float GetSpawnX(int index) const;
    float GetSpawnY(int index) const;

    // ── Controle de portal ────────────────────────────────────────
    bool IsChangingStage()  const { return changingStage; }
    void BeginStageChange()       { changingStage = true; }
    void SetStageChangeCooldown(float time) { changeCooldown = time; }
    void UpdateStageTransition(float dt);
    int  GetCurrentStage()  const { return currentBG; }
    void CreatePortalsForCurrentStage();
    void CreateClosedDoorsForCurrentStage();
    void ClearActivePortals();
    float PortalRotation(float x, float y) const;
    float PortalX(float x) const;
    float PortalY(float y) const;
    int   currentBG = 0;
    float deathFade = 0.0f;

    // ── HUD ───────────────────────────────────────────────────────
    void DrawCentralMessage(const std::string& text, Color color, float x, float y);
    void DrawHeartHealth();

    // ── Estatísticas ──────────────────────────────────────────────
    int   ghostAlive            = 0;
    bool  comeuItem             = false;
    bool  foodSpawned           = false;
    int   totalEnemiesDefeated  = 0;
    int   totalDamageTaken      = 0;
    float totalPlayTime         = 0.0f;
};

#endif
