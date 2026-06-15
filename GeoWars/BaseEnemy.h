/**********************************************************************************
// BaseEnemy (Arquivo de Cabeçalho)
//
// Descrição:   Classe base para todos os inimigos.
//              Herda de Object e implementa IDamageable.
//              Contém física com gravidade, sistema de HP, knockback,
//              drop de geo e integração com tilemap.
//
**********************************************************************************/

#ifndef BASEENEMY_H
#define BASEENEMY_H

#include "Object.h"
#include "IDamageable.h"
#include "Particles.h"
#include "Vector.h"
#include "Physics.h"

// -------------------------------------------------------------------------------

// Estados genéricos dos inimigos
enum EnemyState
{
    ES_IDLE,
    ES_PATROL,
    ES_CHASE,
    ES_ATTACK,
    ES_HURT,
    ES_DEAD,
    ES_ALERT,    // viu o player, vai começar a perseguir
};

// -------------------------------------------------------------------------------

class BaseEnemy : public Object, public IDamageable
{
public:
    BaseEnemy(int maxHp, int geoDrop);
    virtual ~BaseEnemy();

    // Object
    void Update()              override;
    void Draw()                override;
    void OnCollision(Object*)  override;

    // IDamageable
    void TakeDamage(int dmg)   override;
    bool IsAlive() const       override { return hp > 0; }

    int  GetHP()   const { return hp; }
    int  GetMaxHP() const { return maxHp; }

protected:
    // ---- subclasses implementam AI ----
    virtual void UpdateAI(float dt) = 0;
    virtual void DrawSprite() = 0;

    // ---- ataque melee ----
    virtual void Attack();

    // ---- física vetorial ----
    Vector* speed;
    bool     onGround;
    bool     facingRight;
    void     ApplyGravity(float dt);
    void     ResolveTiles();

    // ---- estado ----
    EnemyState state;
    float      hurtTimer;
    float      deadTimer;
    float      alertTimer;
    float      attackTimer;
    float      attackCooldown;

    // ---- stats ----
    int        hp, maxHp;
    int        geoDrop;
    int        attackDamage;
    float      attackRange;

    // ---- half-extents AABB ----
    float      hw, hh;

    // ---- partículas ----
    Particles* deathParticles;
    Particles* hurtParticles;

    // ---- helpers ----
    float DistToPlayer()          const;
    float AngleToPlayer()         const;
    bool  PlayerInRange(float r)  const;
    bool  PlayerInSight(float r)  const;

private:
    bool invincible;
    float invincibleTimer;
    void SpawnGeoPickup();
};

// -------------------------------------------------------------------------------

#endif