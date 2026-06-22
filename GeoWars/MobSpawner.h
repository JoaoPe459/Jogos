#ifndef _MOBSPAWNER_H_
#define _MOBSPAWNER_H_

#include "BaseEnemy.h"
#include "LevelGrid.h"

enum SpawnMobType { SMOB_CRAWLER = 0, SMOB_FLYER = 1 };

struct SpawnPoint
{
    float x, y;           // posição no mundo
    SpawnMobType mobType; // qual mob spawna
    float triggerRadius;  // distância do player para ativar
    float cooldown;       // segundos entre cada spawn
    int   maxPerPoint;    // limite de mobs vivos deste ponto
    bool  active;         // pode ser desligado individualmente

    // estado interno
    float timer;          // tempo acumulado desde o último spawn
    int   aliveCount;     // quantos mobs deste ponto ainda vivem

    SpawnPoint(float px, float py, SpawnMobType t,
        float radius = 400.f, float cd = 4.f, int maxP = 3)
        : x(px), y(py), mobType(t), triggerRadius(radius),
        cooldown(cd), maxPerPoint(maxP), active(true),
        timer(0.f), aliveCount(0) {
    }
};

class MobSpawner
{
private:
    vector<SpawnPoint> points;
    int   globalMax;    // cap total de inimigos no mapa
    bool  enabled;

    float DistToPlayer(const SpawnPoint& sp) const;
    void  SpawnMob(SpawnPoint& sp);

public:
    MobSpawner(int globalMaxEnemies = 20);

    // Registra um ponto de spawn manualmente
    void AddPoint(SpawnPoint sp);

    // Lê pontos 'S' da grade (chame após LoadFromFile)
    void LoadFromGrid(const LevelGrid& grid, SpawnMobType defaultType = SMOB_CRAWLER);

    // Notifica que um mob deste ponto morreu (chame no destrutor do inimigo)
    void OnEnemyDied(float originX, float originY);

    // Atualiza todos os pontos — chame em GeoWars::Update()
    void Update(float deltaTime);

    void SetEnabled(bool v) { enabled = v; }
    int  GlobalMax()  const { return globalMax; }
};

#endif