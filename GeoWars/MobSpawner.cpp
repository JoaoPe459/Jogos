#include "MobSpawner.h"
#include "GeoWars.h"
#include "Crawler.h"
#include "Flyer.h"
#include "Engine.h"
#include <cmath>

MobSpawner::MobSpawner(int globalMaxEnemies)
    : globalMax(globalMaxEnemies), enabled(true) {
}

// ---------------------------------------------------------------
float MobSpawner::DistToPlayer(const SpawnPoint& sp) const
{
    float dx = GeoWars::player->X() - sp.x;
    float dy = GeoWars::player->Y() - sp.y;
    return sqrtf(dx * dx + dy * dy);
}

// ---------------------------------------------------------------
void MobSpawner::SpawnMob(SpawnPoint& sp)
{
    switch (sp.mobType)
    {
    case SMOB_CRAWLER:
    {
        Crawler* cr = new Crawler(sp.x, sp.y);
        GeoWars::scene->Add(cr, MOVING);
        GeoWars::enemies.Add(cr);
        break;
    }
    case SMOB_FLYER:
    {
        Flyer* f = new Flyer(sp.x, sp.y);
        GeoWars::scene->Add(f, MOVING);
        GeoWars::enemies.Add(f);
        break;
    }
    }
    sp.aliveCount++;
    sp.timer = 0.f;
}

// ---------------------------------------------------------------
void MobSpawner::AddPoint(SpawnPoint sp)
{
    points.push_back(sp);
}

// ---------------------------------------------------------------
// Lê células 'S'/'5' da grade e cria SpawnPoints automáticos
void MobSpawner::LoadFromGrid(const LevelGrid& grid, SpawnMobType defaultType)
{
    float half = grid.CellSize() / 2.f;
    for (int r = 0; r < grid.Rows(); r++)
        for (int c = 0; c < grid.Cols(); c++)
            if (grid.GetCell(c, r) == SPAWN_SPAWNER)   // novo enum (veja nota abaixo)
            {
                float cx = c * grid.CellSize() + half;
                float cy = r * grid.CellSize() + half;
                points.emplace_back(cx, cy, defaultType);
            }
}

// ---------------------------------------------------------------
// Avisa que um mob morreu — decrementa o contador do ponto mais próximo
void MobSpawner::OnEnemyDied(float ox, float oy)
{
    float best = 1e9f;
    SpawnPoint* owner = nullptr;
    for (auto& sp : points)
    {
        float dx = ox - sp.x, dy = oy - sp.y;
        float d = sqrtf(dx * dx + dy * dy);
        if (d < best) { best = d; owner = &sp; }
    }
    if (owner && owner->aliveCount > 0)
        owner->aliveCount--;
}

// ---------------------------------------------------------------
void MobSpawner::Update(float deltaTime)
{
    if (!enabled) return;

    // cap global: conta todos os inimigos vivos
    if (GeoWars::enemies.count >= globalMax) return;

    for (auto& sp : points)
    {
        if (!sp.active) continue;

        // acumula cooldown
        sp.timer += deltaTime;
        if (sp.timer < sp.cooldown) continue;

        // verifica proximidade do player
        if (DistToPlayer(sp) > sp.triggerRadius) continue;

        // verifica limite por ponto
        if (sp.aliveCount >= sp.maxPerPoint) continue;

        SpawnMob(sp);
    }
}