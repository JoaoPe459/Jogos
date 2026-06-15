/**********************************************************************************
// BaseEnemy (Código Fonte)
//
// Descrição:   Implementação da classe base para inimigos
//
**********************************************************************************/

#include "BaseEnemy.h"
#include "GeoWars.h"
#include "GeoPickup.h"
#include "IDamageable.h"
#include "Physics.h"
#include <cmath>
#include <algorithm>
using std::min;
using std::max;

// -------------------------------------------------------------------------------

BaseEnemy::BaseEnemy(int maxHp, int geoDrop)
    : onGround(false), facingRight(false),
    state(ES_IDLE),
    hurtTimer(0), deadTimer(0), alertTimer(0),
    invincible(false), invincibleTimer(0),
    hp(maxHp), maxHp(maxHp), geoDrop(geoDrop),
    hw(16.0f), hh(16.0f)
{
    type = ENEMY;
    speed = new Vector(0.0f, 0.0f);

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
    delete speed;
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
    if (invincibleTimer > 0) invincibleTimer -= dt;
    else                     invincible = false;

    // Estado morto: aguarda animação e remove da cena
    if (state == ES_DEAD)
    {
        deadTimer -= dt;
        deathParticles->Update(dt);

        // Aplica queda durante animação de morte
        ApplyGravity(dt);
        Translate(speed->XComponent() * dt,
            -speed->YComponent() * dt);

        if (deadTimer <= 0)
            GeoWars::scene->Delete(this, MOVING);
        return;
    }

    // Estado hurt: física passiva, AI bloqueada
    if (state == ES_HURT && hurtTimer > 0)
    {
        ApplyGravity(dt);
        Translate(speed->XComponent() * dt,
            -speed->YComponent() * dt);
        ResolveTiles();
        hurtParticles->Update(dt);
        return;
    }
    else if (state == ES_HURT && hurtTimer <= 0)
    {
        state = ES_PATROL;
    }

    // AI específica da subclasse
    UpdateAI(dt);

    // Física
    //ApplyGravity(dt);
    Translate(speed->XComponent() * dt,
        -speed->YComponent() * dt);
    ResolveTiles();

    // Partículas
    hurtParticles->Update(dt);
    deathParticles->Update(dt);

    if (x < 50)
        MoveTo(50, y);
    if (y < 50)
        MoveTo(x, 50);
    if (x > game->Width() - 50)
        MoveTo(game->Width() - 50, y);
    if (y > game->Height() - 50)
        MoveTo(x, game->Height() - 50);
}

// -------------------------------------------------------------------------------

void BaseEnemy::Draw()
{
    // Pisca durante hurt
    if (state == ES_HURT)
    {
        if ((int)(hurtTimer * 12) % 2 == 0) return;
    }

    DrawSprite();
    deathParticles->Draw(Layer::MIDDLE, 1.0f);
    hurtParticles->Draw(Layer::MIDDLE, 1.0f);
}

// -------------------------------------------------------------------------------

void BaseEnemy::OnCollision(Object* obj)
{
    if (obj->Type() == PLAYER && state != ES_DEAD && state != ES_HURT)
    {
        Player* p = dynamic_cast<Player*>(obj);
        if (p && !p->IsDashing())
            p->TakeDamage(1);
    }
}

// -------------------------------------------------------------------------------

void BaseEnemy::TakeDamage(int dmg)
{
    if (invincible || state == ES_DEAD) return;

    hp = max(hp - dmg, 0);
    hurtParticles->Generate(x, y, 6);

    if (hp <= 0)
    {
        state = ES_DEAD;
        deadTimer = 0.8f;

        // Impulso vetorial para cima ao morrer
        speed->ScaleTo(0.0f);
        speed->Add(Vector(90.0f, 200.0f));

        deathParticles->Generate(x, y, 20);
        GeoWars::audio->Play(EXPLODE);
        SpawnGeoPickup();
        return;
    }

    // Hurt: knockback vetorial oposto ao player
    state = ES_HURT;
    hurtTimer = 0.35f;
    invincible = true;
    invincibleTimer = 0.4f;

    float knockbackAngle = (x >= GeoWars::player->X()) ? 60.0f : 120.0f; // diagonal p/ cima e p/ fora
    speed->ScaleTo(0.0f);
    speed->Add(Vector(knockbackAngle, 260.0f));

    GeoWars::audio->Play(HURT_SFX);
}

// -------------------------------------------------------------------------------

void BaseEnemy::ApplyGravity(float dt)
{
    if (state == ES_DEAD) return; // morte tem impulso próprio, sem gravidade extra

    // Empurra vetor para baixo (270°)
    speed->Add(Vector(270.0f, Physics::GRAVITY * dt));

    // Limita velocidade de queda
    if (-speed->YComponent() > Physics::MAX_FALL_SPEED)
    {
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        if (curVX != 0.0f)
            speed->Add(Vector(curVX > 0 ? 0.0f : 180.0f, fabsf(curVX)));
        speed->Add(Vector(270.0f, Physics::MAX_FALL_SPEED));
    }
}

// -------------------------------------------------------------------------------

void BaseEnemy::ResolveTiles()
{
    bool onCeiling = false, onWallL = false, onWallR = false;

    // Bordas horizontais
    if (x - hw < 0)
    {
        MoveTo(hw, y);
        float curVY = speed->YComponent();
        speed->ScaleTo(0.0f);
        if (curVY != 0.0f)
            speed->Add(Vector(curVY > 0 ? 90.0f : 270.0f, fabsf(curVY)));
        onWallL = true;
    }
    else if (x + hw > 3840.0f)
    {
        MoveTo(3840.0f - hw, y);
        float curVY = speed->YComponent();
        speed->ScaleTo(0.0f);
        if (curVY != 0.0f)
            speed->Add(Vector(curVY > 0 ? 90.0f : 270.0f, fabsf(curVY)));
        onWallR = true;
    }

    // Bordas verticais
    if (y - hh < 0)
    {
        MoveTo(x, hh);
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        if (curVX != 0.0f)
            speed->Add(Vector(curVX > 0 ? 0.0f : 180.0f, fabsf(curVX)));
        onCeiling = true;
    }
    else if (y + hh >= 2160.0f)
    {
        MoveTo(x, 2160.0f - hh);
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        if (curVX != 0.0f)
            speed->Add(Vector(curVX > 0 ? 0.0f : 180.0f, fabsf(curVX)));
        onGround = true;
    }
    else
    {
        onGround = false;
    }

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
    return atan2f(dy, dx) * (180.0f / 3.14159f);
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

    // Sem tilemap: linha de visada sempre livre
    return true;
}

// -------------------------------------------------------------------------------

void BaseEnemy::SpawnGeoPickup()
{
    if (geoDrop <= 0) return;
    GeoWars::scene->Add(new GeoPickup(x, y, geoDrop), MOVING);
}

// -------------------------------------------------------------------------------
