/**********************************************************************************
// BaseEnemy (Código Fonte)
//
// Descrição:   Implementação da classe base para inimigos
//
**********************************************************************************/

#include "BaseEnemy.h"
#include "GeoWars.h"
#include "IDamageable.h"
#include "Physics.h"
#include <cmath>
#include <algorithm>
using std::min;
using std::max;

// -------------------------------------------------------------------------------

BaseEnemy::BaseEnemy(int maxHp, int geoDrop)
    : velX(0), velY(0),
    onGround(false), facingRight(false),
    state(ES_IDLE),
    hurtTimer(0), deadTimer(0), alertTimer(0),
    hp(maxHp), maxHp(maxHp), geoDrop(geoDrop),
    hw(16.0f), hh(16.0f)
{
    type = ENEMY;

    // ---- partículas de morte ----
    Generator death;
    death.imgFile = "Resources/Spark.png";
    death.angle = 90.0f;
    death.spread = 360.0f;
    death.lifetime = 0.6f;
    death.frequency = 0.0f;
    death.percentToDim = 0.4f;
    death.minSpeed = 80.0f;
    death.maxSpeed = 280.0f;
    death.color = { 1.0f, 0.3f, 0.1f, 1.0f };
    deathParticles = new Particles(death);

    // ---- partículas de hurt ----
    Generator hurt;
    hurt.imgFile = "Resources/Spark.png";
    hurt.angle = 90.0f;
    hurt.spread = 180.0f;
    hurt.lifetime = 0.2f;
    hurt.frequency = 0.0f;
    hurt.percentToDim = 0.5f;
    hurt.minSpeed = 40.0f;
    hurt.maxSpeed = 140.0f;
    hurt.color = { 1.0f, 0.6f, 0.0f, 1.0f };
    hurtParticles = new Particles(hurt);
}

// -------------------------------------------------------------------------------

BaseEnemy::~BaseEnemy()
{
    delete deathParticles;
    delete hurtParticles;
}

// -------------------------------------------------------------------------------

void BaseEnemy::Update()
{
    float dt = gameTime;

    // Decrementa timers
    if (hurtTimer > 0) hurtTimer -= dt;
    if (alertTimer > 0) alertTimer -= dt;

    // --- Estado morto: espera animação de morte e remove da cena ---
    if (state == ES_DEAD)
    {
        deadTimer -= dt;
        deathParticles->Update(dt);
        if (deadTimer <= 0)
            GeoWars::scene->Delete(this, MOVING);

            return;
    }

    // --- Estado hurt: aplica física mas bloqueia AI ---
    if (state == ES_HURT && hurtTimer > 0)
    {

        ApplyGravity(dt);
        //x += velX * dt;
        //y += velY * dt;
        ResolveTiles();
        hurtParticles->Update(dt);
        return;
    }
    else if (state == ES_HURT && hurtTimer <= 0)
    {
        state = ES_PATROL;
    }

    // --- AI específica da subclasse ---
    UpdateAI(dt);

    // --- Física ---
    ApplyGravity(dt);
    //x += velX * dt;
    //y += velY * dt;
    ResolveTiles();

    // --- Partículas ---
    hurtParticles->Update(dt);
    deathParticles->Update(dt);
}

// -------------------------------------------------------------------------------

void BaseEnemy::Draw()
{
    // Pisca em hurt
    if (state == ES_HURT)
    {
        bool visible = ((int)(hurtTimer * 12) % 2 == 0);
        if (!visible) return;
    }

    DrawSprite();

    deathParticles->Draw(Layer::MIDDLE, 1.0f);
    hurtParticles->Draw(Layer::MIDDLE, 1.0f);
}

// -------------------------------------------------------------------------------

void BaseEnemy::OnCollision(Object* obj)
{
    // Projétil do player (AttackHitbox) é tratado via IDamageable
    // Colisão com o próprio player: causa dano no player
    if (obj->Type() == PLAYER && state != ES_DEAD && state != ES_HURT)
    {
        //Player* p = dynamic_cast<Player*>(obj);
        //if (p && !p->IsDashing())   // dash é invencível
        //    p->TakeDamage(1);
    }
}

// -------------------------------------------------------------------------------

void BaseEnemy::TakeDamage(int dmg)
{
    if (state == ES_DEAD) return;

    hp -= dmg;
    hp = max(hp, 0);

    // Partículas de hurt
    hurtParticles->Generate(x, y, 6);

    if (hp <= 0)
    {
        state = ES_DEAD;
        deadTimer = 0.8f;
        velX = 0;
        velY = -200.0f;
        deathParticles->Generate(x, y, 20);
        GeoWars::audio->Play(EXPLODE);
        SpawnGeoPickup();
    }
    else
    {
        state = ES_HURT;
        hurtTimer = 0.35f;
        // knockback: voa para longe do player
        float dx = x - GeoWars::player->X();
        velX = (dx >= 0 ? 200.0f : -200.0f);
        velY = -180.0f;
        GeoWars::audio->Play(HURT_SFX);
    }
}

// -------------------------------------------------------------------------------

void BaseEnemy::ApplyGravity(float dt)
{
    velY += Physics::GRAVITY * dt;
    if (velY > Physics::MAX_FALL_SPEED)
        velY = Physics::MAX_FALL_SPEED;
}

// -------------------------------------------------------------------------------

void BaseEnemy::ResolveTiles()
{
    
    bool onCeiling = false, onWallL = false, onWallR = false;
    /*Player::tilemap->ResolveAABB(x, y, hw, hh,
                                 velX, velY,
                                 onGround, onCeiling,
                             onWallL, onWallR);
                             */
                             // Inverte patrulha ao bater na parede
    if (onWallL || onWallR)
        facingRight = onWallL;
}

// -------------------------------------------------------------------------------

float BaseEnemy::DistToPlayer() const
{
    float dx = GeoWars::player->X() - x;
    float dy = GeoWars::player->Y() - y;
    return sqrtf(dx * dx + dy * dy);
}

// -------------------------------------------------------------------------------

float BaseEnemy::AngleToPlayer() const
{
    float dx = GeoWars::player->X() - x;
    float dy = -(GeoWars::player->Y() - y);   // eixo Y invertido
    return atan2f(dy, dx) * 180.0f / 3.14159f;
}

// -------------------------------------------------------------------------------

bool BaseEnemy::PlayerInRange(float range) const
{
    return DistToPlayer() < range;
}

// -------------------------------------------------------------------------------

bool BaseEnemy::PlayerInSight(float range) const
{
    if (!PlayerInRange(range)) return false;

    // Raycasting simples: verifica tiles entre inimigo e player
    //if (!Player::tilemap) return true;

    float px = GeoWars::player->X();
    float py = GeoWars::player->Y();
    int steps = 12;
    for (int i = 1; i < steps; ++i)
    {
        float t = (float)i / steps;
        float rx = x + (px - x) * t;
        float ry = y + (py - y) * t;
        /*if (Player::tilemap->GetAt(rx, ry) == TILE_SOLID)
            return false;*/
    }
    return true;
}

// -------------------------------------------------------------------------------

void BaseEnemy::SpawnGeoPickup()
{
    //if (geoDrop <= 0) return;
    //GeoWars::scene->Add(new GeoPickup(x, y, geoDrop), MOVING);
}

// -------------------------------------------------------------------------------
